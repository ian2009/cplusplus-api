/*
-----------------------------------------------------------------------------
Copyright (c) 2010 Tippett Studio
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

#include <iostream>

#include <Shotgun/TaskMixin.h>
#include <Shotgun/Shotgun.h>
#include <Shotgun/Entity.h>
#include <Shotgun/Show.h>
#include <Shotgun/User.h>
#include <Shotgun/Group.h>

namespace Shotgun {

// *****************************************************************************
Tasks TaskMixin::getTasks(const int limit)
{
    // Dynamic_cast can cast it to the other base class of this class' 
    // derived class.
    if (Entity *entity = dynamic_cast<Entity *>(this))
    {
        return entity->sg()->findTasksByLinkEntity(entity->sgShowCode(), 
                                                   entity->asLink(),
                                                   limit);
    }
    else
    {
        throw SgEntityDynamicCastError("Entity");
    }
}

// *****************************************************************************
Task TaskMixin::getTaskByName(const std::string &taskName)
{
    if (Entity *entity = dynamic_cast<Entity *>(this))
    {
        SgMap findMap = Entity::buildFindMapWithNoFilter(entity->sg(),
                                                         "Task",
                                                         entity->sgShowCode(),
                                                         1);

        Entity::addOneConditionToFindMap(findMap, "content", "is", toXmlrpcValue(taskName)); 
        Entity::addOneConditionToFindMap(findMap, "entity", "is", toXmlrpcValue(entity->asLink()));

        Tasks tasks = entity->sg()->findTasks(findMap);
        if (tasks.size() > 0)
        {
            return tasks[0];
        }
        else
        {
            throw SgEntityNotFoundError("Task");
        }
    }
    else
    {
        throw SgEntityDynamicCastError("Entity");
    }
}

// *****************************************************************************
Tasks TaskMixin::getMilestoneTasks(const int limit)
{
    if (Entity *entity = dynamic_cast<Entity *>(this))
    {
        SgMap findMap = Entity::buildFindMapWithNoFilter(entity->sg(),
                                                         "Task",
                                                         entity->sgShowCode(),
                                                         limit);

        Entity::addOneConditionToFindMap(findMap, "entity", "is", toXmlrpcValue(entity->asLink()));
        Entity::addOneConditionToFindMap(findMap, "milestone", "is", toXmlrpcValue(true)); 

        return entity->sg()->findTasks(findMap);
    }
    else
    {
        throw SgEntityDynamicCastError("Entity");
    }
}

// *****************************************************************************
Task TaskMixin::getNextIncompleteMilestoneTask()
{
    Tasks milestones = getMilestoneTasks();

    int next = -1; 
    for (size_t i = 0; i < milestones.size(); i++)
    {
        // Is this milestone incomplete?
        if (milestones[i].sgStatus() != "cmpt")
        {
            // Yes, consider it
            if (next < 0 or 
                milestones[next].sgEndDate() > milestones[i].sgEndDate())
            {
                next = i;
            }
        }    
    }

    if (next > 0)
    {
        // Return the last one found that has the ealiest endDate
        return milestones[next];
    }
    else
    {
        throw SgEntityNotFoundError("Task");
    }
}

// *****************************************************************************
Task TaskMixin::addTask(const std::string &taskName,
                        const std::string &taskType,
                        const int taskViewOrder,
                        const std::string &taskAssignee,
                        const std::string &taskStartDate,
                        const std::string &taskEndDate,
                        const std::string &taskStatus,
                        const std::string &taskColor,
                        const bool taskMilestone)
{
    // Check if the task already exists
    try
    {
        // TODO: check "task_mixin.py"
        return getTaskByName(taskName);
    }
    catch (SgEntityNotFoundError)
    {
        if (Entity *entity = dynamic_cast<Entity *>(this))
        {
            return entity->sg()->createTask(entity->sgShowCode(),
                                            taskName,
                                            taskType,
                                            taskViewOrder,
                                            taskAssignee,
                                            taskStartDate,
                                            taskEndDate,
                                            taskStatus,
                                            taskColor,
                                            taskMilestone,
                                            entity->asLink());
        }
        else
        {
            throw SgEntityDynamicCastError("Entity");
        }
    }
}

// *****************************************************************************
Task TaskMixin::updateTask(const std::string &taskName,
                           const int taskViewOrder,
                           const std::string &taskAssignee,
                           const std::string &taskStartDate,
                           const std::string &taskEndDate,
                           const std::string &taskStatus,
                           const std::string &taskColor,
                           const bool taskMilestone)
{
    Task task = getTaskByName(taskName);

    SgMap updateMap;

    // taskViewOrder
    if (taskViewOrder != TIPSHOTGUN_INVALID_ORDER_NUM)
    {
        updateMap["sg_view_order"] = toXmlrpcValue(taskViewOrder);
    }

    // taskAssignee
    if (taskAssignee != "")
    {
        try
        {
            User user = task.sg()->findUserByLogin(taskAssignee);

            SgArray assignees;
            assignees.push_back(toXmlrpcValue(user.asLink()));
            updateMap["task_assignees"] = toXmlrpcValue(assignees);
        }
        catch (SgEntityNotFoundError)
        {
            try
            {
                Group group = task.sg()->findGroupByName(taskAssignee);

                SgArray assignees;
                assignees.push_back(toXmlrpcValue(group.asLink()));
                updateMap["task_assignees"] = toXmlrpcValue(assignees);
            }
            catch (SgEntityNotFoundError)
            {
                // Do nothing
            }
        }
    }

    // taskStartDate
    if (taskStartDate != "")
    {
        if (taskStartDate == "now")
        {
            updateMap["start_date"] = toXmlrpcValue(currDateStr());
        }
        else
        {
            updateMap["start_date"] = toXmlrpcValue(taskStartDate);
        }
    }

    // taskEndDate
    if (taskEndDate != "")
    {
        if (taskEndDate == "now")
        {
            updateMap["due_date"] = toXmlrpcValue(currDateStr());
        }
        else
        {
            updateMap["due_date"] = toXmlrpcValue(taskEndDate);
        }
    }

    // taskStatus
    if (taskStatus != "")
    {
        updateMap["sg_status_list"] = toXmlrpcValue(taskStatus);
    }

    // taskColor
    if (taskColor != "")
    {
        updateMap["color"] = toXmlrpcValue(taskColor);
    }

    // taskMilestone
    updateMap["milestone"] = toXmlrpcValue(taskMilestone);

    // update the task's attributes
    task.setAttrValue(updateMap);

    return task;
}

// *****************************************************************************
bool TaskMixin::removeTask(const std::string &taskName)
{
    try
    {
        Task task = getTaskByName(taskName);

        task.sg()->deleteTaskById(task.sgId());

        return true;
    }
    catch (SgEntityNotFoundError)
    {
        return false;
    }
}

} // End namespace Shotgun