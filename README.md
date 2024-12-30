# mockberry-bc
this is a mock implementation to test the design of boundary conditions for FoxBerry and DaRe

# Motivation
Immersed Boundaries and Domain boundaries are kinda doing the same, with the difference that domain boundaries are in principle a special case of the immersed boundaries and therefore a bit simpler. Keeping them separate leads to lots of shenanigans when an immersed object comes near a boundary. However, right now the immersed boundaries are a huge complexity monster that should be taken apart and requires an extreme effort to try out new implementations. Additionally, the objects should be managed outside of the immersed boundaries, e.g. for moving objects. Therefore a new design is required, which separates the objects from the boundary conditions, and allows for better separation of applications.

# Ideas
- All objects should be managed by an `ObjectManager`. The objects my require external management for communication, updating and serialization.
- Using the polymorphic approach makes it quite unhandy to deal with new objects or updates, use external polymorphism instead by means of a type erasure
- The way **how** a boundary is applied should be variable, meaning each equation should have a dedicated structure to do so
- Updating of the various parameters should be taken care of in the background. The user should not be bothered with doing that and keeping the order

# Solution
Firstly, a type erasure for the objects is defined, which can be **owning** or **non-owning**. Those can then be managed by the object manager.
Secondly, the object manager and subsequently the other methods implement **observers**, which can be notfied once certain functions are called.
Each equation now holds a `BCMethod`, which defines the types of boundary conditions the user is allowed to povide. Those BCMethods rely on a `BoundaryLocator` (e.g. the former flag field) for optimization. The `BoundaryLocator` has an observer attached to the `ObjectManager`, and also provides observers. In this way, the `ObjectManager` notifies the `BoundaryLocator`, which can update and in turn notify the `BCMethod` (e.g. to precompute extrapolated values). 
