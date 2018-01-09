# Deviations

### 1 
We have no regression tests. This is an effect due to how late we finished our unit tests way later than planned, making the regression tests not possible to make unless we faked them. We feel that for the next time we have a project like this we will definately make our unit tests ASAP and then do regression tests every step of the way. Simpifying the entire process. We feel that the issues we had could have been avoided if we were stricter with deadlines, follow up on things and better communication in general.

### 2 
Git issues log, for some reason we have completely forgotten to do issue reports. This is due to several reasons some being that we had some people doing pair-programming who instead of making a pull request for something that did not work would rather work it out amongst them selves until it worked. We also had a similar thing with others in the group where they would not make a pull request or push anything they knew didn't work. So the few times we actually had something worth making an issue of we simlpy forgot. For the future we will all try to make issue reports due to how it can notify everyone in the group in a simple manner about a bug so that they know that it is there and it needs fixing.

### 3
The unit test for deallocate is not done. This is merely due to it not being possible to make a good test for it since the only way to see if it has done what it is supposed to do will result in a segfault. This is because the things we will try to check will have been freed resulting in us not being able to access it any longer.
