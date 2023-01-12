# Traffic Simulation
## Description
I will be trying to simulate traffic using a Direcitonal Graph and a Behavior Tree.

The code is made using a c++ framework made by our teachers. 

All PathFinding is done using A*.

### Gif of the Result
![Results_Image](/Assets/Images/Gifs/Result_TrafficSimulation.gif)


## Implementation
### Step 1 - Creating the grid

We want to start off with creating a grid where our cars can travel on.

For this first simulation a grid is good enough, we just want to achieve basic movement/decision making.
Using a grid will make our city look like New York, so why not.

When launching the program right now we will be getting this screen.
![Startup_Grid_Image](/Assets/Images/Gifs/Step1_DefaultGrid.gif)

This however is code that was given to us by our teachers.

We have a Start and End point that we can move and a path that will be calculated using A*.
The code for A* is something that we wrote ourselves during class.
It's nice that we have a grid ready to use, but there are some things missing.

The grid makes a graph, but is by default an Undirected Graph.
When working with streets, we don't want this.


If we take a look at the types of streets there are, we can deduce 2 different sorts.

1: a One-Way street
  - This is easy, just one lane and cars cannot go the opposite direction.
  
2: a Two-Way street
  - This one has 2 lanes (not visible everytime), going in the opposite direction.
  - Legally, on a Two-Way street, it's not allowed to just turn around and go to the other direction. (An exception could be an intersection.)

So a Directed Graph suits this program a lot better.
We will have a clear view of each street and Two-Way streets will be bigger.

Let's get started on creating this grid!


But before that; what should the connection cost be of the Graph?

I decided to go with a value that could be used in km/h. This might change the speed of our car later on.

After making connections and adding building to our grid, we have this problem.
![First_Grid_Problem_Image](/Assets/Images/Gifs/Step1_FirstGridProblem.gif)

At first sight, this looks good. But when looking closer, the path always uses a U turn.

I would prefer it when the cars take a path that doesn't use any U turns.

We can easily fix this; we need to change the costs of the Intersection Connections.

Doing this will massively increase the cost of every Intersection taken, so our path will have to go around.

Obviously we don't want to keep this cost change, so we only increase it before calculating the new path, and instantly decrease it after calculating it.


Doing so will result in this! It looks a lot cleaner now.

![First_Grid_Problem_Image](/Assets/Images/Gifs/Step1_FirstGrid.gif)

At times our path will contain a U turn, but this only happens when the path is really small. <br /> <br />

### Step 2 - Adding a car

#### ***The "Car" Class***
<br />
<br />
We need a way to render and move our Cars, for this we will create a Car Class.

It inherits from the SteeringAgent Class from our teachers. This one works with a RigidBody system that uses physics to move.

In this class we will be making a SteeringBehavior that Uses the "Seek" methode. It will calculate a steering based on the next point in our path.

This class will also contain our path, which will be a list of Points. We want to visit every Point in this list, after that calculate a new path.<br /><br />

#### ***The Behavior Tree***
<br />
<br />
When creating the car, we will need to give it a way to control itself. We do this by using a Behavior Tree.

This Behavior Tree will not be complicated. When updating our BT, we will be checking all these options and excecuting the first that returns true:

  - If we have arrived at our final position; we calculate a new path.
  - If we are at our next position in our path; we remove this from our path and seek the next point;
  - we change our speed depending on the connection cost.
  
 These are 3 really simpel things to do, that will cause our car to move. <br /> <br />

 #### ***Spawning the Car***
 <br />
 <br />
 We have our Car class and a BehaviorTree that will be capable of moving the car; so now we just spawn them.
 We need to spawn the Car at the center of a random Cell of the Grid. Prepare some safety checks; we don't want a car to spawn in a building, and we might aswell take care of keeping track in which Cell a car has spawned. This will be used when spawning multiple cars to avoid overlap on spawn. <br /> <br />
 
 When a starting position has been decided, we can make the end Position the same as our start. Our Behavior Tree will see that we are in the final cell and will calculate a new path for us! <br /><br />
 If we are going for random things; might aswell give the car a random color.
 
 
 After doing all these small things; we can finally see a car moving!
 
 ![Moving_First_Car_Image](/Assets/Images/Gifs/Step2_FirstCar.gif)
 
 The car is moving really slow, because of the RigidBody system it has a hard time turning, so making it go faster will make the car go off track.
 
 Slowing the car down by 4 times is currently the only option. <br /> <br />

### Step 3 - Making Multiple Cars Aware <br /> <br />

Spawning multiple cars is not a problem, thanks to my code I just have to change the value of an integer; m_AmountOfCars.

So lets's do that!

 ![Multiple_Cars_Problem_Image](/Assets/Images/Gifs/Step3_MultipleCars.gif)
 
 Well then...
 
 At least the cars spawn correctly, but there is something really obviously wrong here. <br />
 **They are bumping into each other...** <br /><br />
 
 We obviously need to fix this; add a way to detect other cars in our Behavior Tree. <br />

  - If we have arrived at our final position; we calculate a new path.
  - If we are at our next position in our path; we remove this from our path and seek the next point;
  - ***Do we need to give way to other cars; Slow down***
  - we change our speed depending on the connection cost. <br /> <br />


But how do we check if we need to give way to other cars?

To start off, we make a cone of vision where we can detect other cars just like this.

 ![ConeOfVision_Image](/Assets/Images/Stills/Step3_VisionCone.png)
 
 
Remeber, give way to cars coming from the right. They have a priority.

To see if the car is coming from the right we need to check the angle between the Direction Vector of each car. <br />
There are 4 possible outcomes with this angle:

  - The angle is ~0; the other car is looking in front of us.
  - The angle is ~Pi || ~-Pi; the other car is looking towards us.
  - The angle is ~-Pi / 2; the other car is to our left.
  - The angle is ~Pi / 2; **the other car is to our right.**  <br /> <br />


There are 2 cases where we need to slow down:
  
  - When the car is to our right; we need to stop moving.
  - When the car in front of us is too close; we need to stop moving.

Before slowing down, make sure to check that the other car is moving. Doing this check will prevent us from stopping while other cars are at a red light.


When we set our MaxLinearSpeed to 0.0f to slow down, we can see that our cars will stop when needed!

 ![Cars_GivingWay_Image](/Assets/Images/Gifs/Step3_GivingWay.gif)

<br /> <br />

### Step 4 - Adding Traffic Lights <br /> <br />


One final thing to add that will make this city look more lively are Traffic Lights.

These shouldn't be hard to implement.

So what does a Traffic Light need?

  - A timer for when to change the light
  - A bool to see if the light is red
  - 2 nodeIndexes too make a connection so we can change the value. <br />


We only want to have traffic lights around our intersections, and let's make them change color every 8 seconds.

The bool will be used to change the Connection Cost between the 2 nodes; when it's red, change it to 0 otherwise put it back at 50.<br /> <br />

We can then draw these lights to show if they work. 

 ![Working_Traffic_Lights_Image](/Assets/Images/Gifs/Step4_TrafficLights.gif)
 
 And they do!
 
 It might look like some cars are running it, but as long as they stop on the node of the light, they are following the rules. <br /> <br />
 
 
 
## Result

Here we have a few seconds of our traffic system in full display!

![Results_Image](/Assets/Images/Gifs/Result_TrafficSimulation.gif)


## In Conclusion

And that is about it, thank you for reading this far!

This might be a very basic traffic system, but it might be a start to something greater. 

It was very fun to work on this and would do it again in a heartbeat. <br /><br />

There are 2 major things I learned out of this quick exercise:
 
  - It's a nightmare to debug a Behavior Tree of multiple entities.
  - Working with something Physics-based is not easy to get started with. <br /><br />



### What could be improved on?
If you run the code yourself, you might notice that there are still some things left unaccounted for; some crashes might still occur.

*(I mean car crashes, not application crashes)* <br /><br />

The physics system of the framework I used is still very foreign for me, exploring this more would help improve everything.

Right now the roads are still just straight, adding turns and twists might be a fun addition yet could be challenging.

There are some formulas that should be applied; eg. the distance to keep between cars,...  <br /><br />

If there is anything you think might improve this Application; feel free to contact me!







