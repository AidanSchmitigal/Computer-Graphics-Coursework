Project #3 Lighting a Scene
For this project there were a lot of calculations to get everything into the correct place. I used the same tequnike as the forward kinematics that we discussed in class. For each planet. Moon, and satellite I have variables for the distance from the parent, orbit speed, and body radius. I start with Earth and calculate where it would be, then while keeping the transformations I calculate where the Moon / ISS will be and translate. This makes it so the moon is centered on the planet. For the ISS and Europa Clipper, I have different scaling on the sin/cos to make it an ellipse.

For the lights, I use the same position calculation for the planets and just set the light position above that. I have variables for the colors and an if statement for the point / spot toggle.

[https://media.oregonstate.edu/media/t/1_invvxfdh](https://media.oregonstate.edu/media/t/1_invvxfdh)
