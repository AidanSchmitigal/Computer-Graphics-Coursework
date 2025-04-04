Project Final The Finale!!

My Proposal:

My big idea: A Subset Implementation of Blender’s Geometry Nodes

My goal with this project is to recreate a subset of Blender's Geometry Nodes system. I want to focus only on the essential nodes to do procedural modeling and animations with transformations. Specifically, I will implement nodes for Math, Transformations, Instancing, and Basic Geometry Manipulation.

For the other 100pt projects, I have gone above and beyond by making other unnecessary additions. For example in project 4 I also made a particle system and modeled all of the objects myself in Blender. I think this follows that trend of doing extra for no reason. In this project I will be able to create any other unnecessary addition my mind can come up with, but this time, using nodes!

If you're worried about the 1-week time frame, I can reassure you the node-graph implementation is already done, and I have used it before. The only thing I will be focusing on is the graphics and geometry nodes. I will be using the [DearImGUI](https://github.com/ocornut/imgui) library with this [ImGUI Node Editor Addon](https://github.com/thedmd/imgui-node-editor).

What I did:

I did almost everything my I said in my proposal. I added some extra features but I did everything I said I would. I setup a GUI using DearImGui and used the ImGuiNode Editor Addon to draw the nodes. Then I implemented the full node graph system and functions. It was so much fun to put together the node graph an learn how things like the evaluation order work in such a connected network. I then implemented all of the functional nodes starting with the primitives. I copied a bunch of the OSU primitives into nodes. Then, I created a vector math and vector construction nodes. I also create simple math nodes for all kinds of math. Lastly I add scene time by making it a global variable in the graph.

To make it save and load your graph I used a library called Cereal to convert all of the nodes and connections into a binary format. Then load them back in by reading the data in the same order.

Differences:

The only difference is I did more. I added the light configuration panel and a debug renderer. I also added more nodes than I anticipated. Overall, nothing much has changed I just decided to do more.

Impressiveness:  
The way I render the scene in ImGUI is using a frame buffer, but frame buffers arn’t in this version of OpenGL on my Mac, so I jerry-rigged together a solution so that it renders to the regular default built in frame buffer then copies the entire frame into a texture pointer. This texture is then rendered as an image in ImGui.

I also made a Torus Knot.

What I learned

I learned how to interface with ImGui better. I have used Java binding but to use the original C++ is so much better. I also learned how to do graph networks and evaluation. This was a tricky thing to get right, there were so many moving parts that each node kept track up. Updating each node to cache their results and only recalculate then they needed to was a fun challenge and increased the performance.

Link:  
[https://media.oregonstate.edu/media/t/1_4pbkcnoi](https://media.oregonstate.edu/media/t/1_4pbkcnoi)
