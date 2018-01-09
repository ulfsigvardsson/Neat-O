# 1 Group name

## 1.1 Participant list
```
 Name               email                        active dates
 Albin Barchéus     alba6827@student.uu.se       5/12 -17 -- 1/1 -18
 Markus Sjöholm     masj8314@student.uu.se       8/12 -17 -- 1/1 -18
 Odd Jensen         odje5470@student.uu.se       8/12 -17 -- 1/1 -18
 Oskar Lidin        osli7853@student.uu.se       8/12 -17 -- 1/1 -18
 Sebastian Lhådö    selh2555@student.uu.se       8/12 -17 -- 9/1 -18
 Ulf Sigvardsson    ulsi6345@student.uu.se       5/12 -17 -- 9/1 -18
```
## 2 Quantification
The project started on 06/12-17 and it ended on 09/01-18. Since we used Kanban we decided that we would not use sprints but would rather work on something until it ended so either we had 0 sprints or 1 sprint depending on how you view it.
Our main scripts with the code for the garbage collector has 641 lines of code.
Our tests (both unit and integration) has 340 lines of code and our makefiles has 31 lines.
We have 123 commits and we have spent 161+ hours working on this project. We have 5 PR and 0 issues due to us missing to use issues.


# 3 Process

## 3.1 Inception
We used the Kanban-process. We did this because we felt it would be the easiest one to follow without forcing us to have sprint-meeting all the time like the Scrum-process. Learning this process was delegated to Albin who later taught the rest  
We implemented our process with a Trello board as this somewhat mirrored using a whiteboard. This provided a clear overview of work to be done and made assigning tasks easy.

## 3.2 Implementation 
Using a digital message board made communication dependent on team members actually checking updates and assignments, which did not always happen. In essence, planning was lacking after the initial setup and tasks were not given any time constraints, this made it very hard to determine whether or not we were on schedule. 

Decisions were made by major vote. We agreed on a soft deadline before christmas, a hard deadline at the hand-in date, the time each member should dedicate each day, the process to use and how to distribute initial tasks. The daily time spent was unfortunately not followed by the team, likely due to a lack of physical meetings. During the christmas break, the group fell into a bit of radio silence and we lacked a clear plan as to what to accomplish during it.

Now in hindsight if we did it all over again we would without a doubt in our minds choose Scrum over Kanban. This is mainly due to how we view the effectiveness of actual meetings over internet communications and how this could motivate us to set stricter time-limits for each part of the project making us more effective in the long run.

# 4 Use of tools:
For coding, we made use of the tools gcov, indent and astyle. The first for profiling our program and the latter ones for formatting the code to conform to our coding standard. gcov provided a picture of where our program spent the most of its time, but we couldn't make any use of the information to make any adjustments as it mostly called the functions that were excpected. indent and astyle were of great use. Without it we would have to manually check all code. 

For communication we used Trello for assigning tasks and measuring process, Slack for day-to-day communication, google.spreadsheet for time reporting and google.questionaire for scanning the mood of the team on a weekly basis. Trello was helpful with providing an overview of our progress and Slack was our main channel of communication and naturally very useful.

google.questionaire was later dropped due to feedback concerning its effectiveness.

We don't think that we were missing any tools in particular.

# 5 Communication, Cooperation and Coordination
Everyone in the group feels like we have had decent communication in some aspects and that we have been lacking in other ways. We have been using slack as our major channel of communications but we feel that more in person communication would have been preferred. We feel that slack is great in many ways but you never know, due to how life is, when the person/people you are
trying to get a hold on will be able to see and/or answer what you wrote. This is one of the reasons why in person meetings are great due to how you definitely know that the people there will be able to respond to what you have to say. 

We had some group members who were very bad at communicating with the rest until quite close to the deadline that it ultimately lead to someone actually being asked to leave the group due to not contributing their part. This is very different depending on which group member is asked. Due to how we split the tasks up some have had great communication whilst the others might not have experienced it in the same manner. As an example. we had some people who had great communication in combination to their pair-programming whilst some who were not doing pair-programming were having a harder time communicating with each other.

We all feel that when we have been in need of cooperation within the team with respect to technical tasks we could always ask each other for help/assistance if we needed it. We did on the other hand have a hard time getting some things 
done due to some miscommunication and bad cooperation between some parts of the group. 
We all met up for pseudo-weekly meetings where we had democratic votes for all the major decisions. For smaller decision we merely had a conversation on slack where everyone voiced their view and then we went with what the majority wanted. 


# 6 Work Breakdown Structure
The tasks were divided in such a manner that we all discussed the problems and then either chose or were delegated some work (depending on if a person attended a meeting) making sure that we were in small subgroups that were to specialize their attention towards one part of the project. The implementation subgroup were using pair programming and felt that it worked surprisingly well for them! Some were very sceptical to begin with and were quite turned in their view of this work method. 

For tasks that were larger than others we split it up so more people worked on those than on the small things. The estimation of time for a task can be very tricky when you are unexperienced in the matter, as we are, so for us it was often approximating and guessing in a broad way.

# 7 Quality Assurance
We feel that the spec for the code is quite clear but when it comes to the specs for the project in general it is very difficult to sometimes understand what is asked of us. Some things are hidden away, not exactly clear or just not mentioned enough. We made sure to read everything a few times just to make sure we didn't miss things.

We wanted to write our unit tests as soon as possible and make sure that our code always succeeded on them to make sure that every new implementation worked. We used a different test-file for our integration tests to make sure that we never had non-working code, but in all honesty this was a makeshift solution while waiting for the unit tests. We, due to the unit tests not beeing completed until the final days of the project, didn't do regression testing which we regret not having done. In retrospective we should have put a clear time constraint on all tasks to prevent them from lingering.

We strongly feel that PR and code review is a positive thing that could only help the project. The only negative thing about it is when people do not go through the PR and review it. But when everyone does their part and thoroughly goes through the code each time a PR is made it gives the readers an understanding of the code which they otherwise might not get. It also gives a sort of relieving feeling to the author of the code that if  something is wrong/bad it will most likely be found so it could be fixed before a merge with the master branch is made. We never really had specific names for our PR, we assumed everyone would read it and understand what it was about. In hindsight we should probably have used more specific names for each PR.

# 8 Reflection
The group voted that we have a satisfaction of 3 when it comes to the process, we all feel that we could have used a better process that uses sprints in a more constructive manner than what we used now. 
We all have a high satisfaction for our product and view it with a satisfaction of 6 for the delivered product.
We also feel that due to our tests we feel some confidence in the quality assurance but as with all tests, you can never know if the code is without bugs, you can merely find them. This gives us a satisfaction of 4.

We think that the biggest win was finding out how amazing pair-programming is since it surprised all of us just how well it worked.

The biggest fail was without a doubt the bad planning we had. We should have had much stricter hold on when things were to be done and by who as well as holding each other accountable.
