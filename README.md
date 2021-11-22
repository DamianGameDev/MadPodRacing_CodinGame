# WolcenTestRepo

Repository to show progress of the test on codingame rankings

## Development decisions and toughs on each stage of the test (Mad Pod Racing)
### League Wood 3, meeting the interface 
* This part doesn't really has much mystery it was only do as the instructions said, but helped me get acquainted with the interface and activity
* Just changed the Thrust value and that got me to the next league

### League Wood 2, New rule
* Same as the first part, just did as suggested
* added thrust change upon angle reaching certain value

### League Wood 1, new mechanic Boost
* For the new mechanic, I started by testing what it did by adding the Boost line
* Once I properly knew what it did, I started thinking on the most optimal way to use it, so I decided to store the longest distance at which my pod was once a checkpoint was reached
* Upon reaching 4 checkpoints the Boost ability could be used, this so the Pod could have at least 4 Distances, compare them and store the longest distance
* Also added a deceleration once near the checkpoint to lessen the drift
* The problem I found here was that since I was storing the location of my pod at the time a checkpoint was reached, this distance would not really get to repeat most of the time so I decided to implement the IsNearlyEqual function, this received a tolerance which I used to validate if the Pod distance to the new checkpoint was at least near the longest distance, with this I was promoted to Bronze

[SPENT 2 HOURS ON THIS PART]

### League Bronze, new mechanic Pod collisions
* At this Point I started to see that it was not optimal nor organized to have all my code inside the main function, so I decided to change my code structure and improve some of my function while doing so
* Made different classes to represent the actors in the game and a manager to make access to the checkpoints needed (Created classes: BaseActor, Checkpoint extends BaseActor, Pod extends BaseActor and CheckpointManager) and improved function that i saw were not working right

[SPENT AT LEAST 1 HOUR]

* Once I created this classes I must confess that I got biased by looking at the suggested documentation on neural network, I decided to try and implement a binary tree to score the best possible thrust depending on my pod angle but it was taking so much time and I wasn't getting anywhere, so I decided to ditch neural networks.

[SPENT 3 - 4 HOURS HERE WITH NOT MUCH ADVANCE]

* Back on track I started by looking at the actual behavior of my pod and identify why it was not as fast and the main problem I identified was the Pod drifting too far once a checkpoint was reached and also the speed too heck and the pod would not manage to reach the checkpoint and passed straight
* With both problems Identified I decided to first attack the drifting problem, the first thing a tried was was using the unit direction vector and adding an offset to the target destination, this offset would be the checkpoint's radius so the pod would start to diminish its thrust earlier, I took this decision since the rules stated that the pod's center had to touch the checkpoint so it was not necessary for the pod to get to the center of the checkpoint, The problem with this approach was that sometimes once the pod reached the location it tried to adjust its rotation causing it to lose momentum on straight checkpoints
* With the UnitDirectionVector problem discarded I decided to try something similar (an offset to the target location) by using the pod's velocity, so depending on the pod's speed this this offset would adapt, this helped the pod get aligned better and drift less getting almost every time the checkpoint, also the problem with the pod trying to rotate losing momentum still happened here but the rotation was a lot less, adding a multiplier here helped, it did not make the problem disappear but the result was better (Maybe if the multiplier had certain criteria to change values it would have helped)
* With the driftin problem fixed I decided to pay attention to the Pod's arrival to the checkpoint, it was no longer getting ahead or drifting but it was losing so much speed, making the rotation a bit slower too so I decided to make the floor of my angle multiplier higher than 0 (taking a maximum of 90% of the velocity)
* Before testing my code on the arena I decided to improve the structure a bit more by leaving the classes and functions more organized and commented
* As a final note here, I had to hardcode the first to frames since my velocity was really weird the first to framed, making the car rotate at the start towards the corner of the map losing 2 precious frames of speed

[SPENT 4 - 5 HOURS]

### League Silver and League Gold, new mechanics Shield(haven't used it yet) and two more pods in the game (one for me and one for the enemy)
* When I reached silver, I stopped working on the test to come back and found that I got promoted to gold so I missed the opportunity to properly use the Shield mechanic
* So once Promoted to gold Looking at the new rules I Had to update my hole code since some of the variables I was getting by using my functions and manager were now given to me for free, which was a good thing because that fixed my two first frames problem
* I decided to improve even more my structure so everything related to Pods is done on my Pods class, not in main (the out inputs and speed offset) or in my checkpoint manager (BOOST validation)
* Improved my Checkpoint manager since the checkpoint were given before the game loop
* And finally made that everything was possible and scalable with more than one pod
* For this part I think I spent maybe a bit more than 15 Hours so I decided to just leave both my pods working individually as it was working before getting the new pod

[SPENT 2 - 3 HOURS REFACTORING CODE SO IT WORKED WIT > 1 POD]

* The only problem I found here was that now the Angle given to me was not the angle of my pod relative to the checkpoint, instead it was the world rotation of my pod, which didn't helped me much so I had to find the relative angle for my thrust multiplier to work right, to do this I had to normalize the position of the pod and the nex checkpoint, getting the dot product and using the atan2 to get my angle (it was on radians so I had to convert the value to degrees) and finally depending on the world angle of the Pod add 360 to the final angle, there was a big problem here with this angle so i had to add 3 degrees more to the difference

[SPENT 2 HOURS REVIEWING AND IMPLEMENTING SOME TRIGONOMETRIC FUNCTIONS/EQUATIONS TO GET THE ANGLE RIGHT]

### Possible thoughts and Improvement Possibilities
* It would have been better if I had used the UnitDirectionVector instead of the angle on that last problem I had
* For the shield use I would have used it on the proximity of the opponent's Pod since this was the reason I lost some of the races
* For the second Pod I might have wanted to do a subclass or maybe in the same pod class so this pod works as a support obstaculizing the opponent's pods and making sure not to collide with the main pod
* Of course Neural network would have worker a lot better since the pods decisions would be taking by scoring the most optimal options but that would require predictions both of my pods and the opponent's pods movements, this could take much time and might be overkill
* Drifting could be improved highly by thrusting on the most optimal angle or even altering thrust intensities while the pod is rotating

[1 HOUR DOCUMENTING THIS]

### FINAL RANK - GOLD 1744/6968

Final Note: The test was really fun, I might keep try and get higher, also didnt know the CodinGames page it is really good to study and go over some concepts among other things
