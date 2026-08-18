#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "chunk.h"
#include "coordinate.h"
#include "entity.h"
#include "tile.h"
#include "vector_2d.h"
#include "world.h"

namespace py = pybind11;

PYBIND11_MODULE(_core, m)
{
     m.doc() = "WorldForgeZero PocketWorld Engine (C extension).";

     // Константы
     m.attr("ENTITY_SOLID") = static_cast<uint32_t>(ENTITY_SOLID);
     m.attr("ENTITY_FLOOR") = static_cast<uint32_t>(ENTITY_FLOOR);
     m.attr("ENTITY_EPHEMERAL") = static_cast<uint32_t>(ENTITY_EPHEMERAL);

     // Coordinate
     py::class_<Coordinate>(m, "Coordinate")
         .def(py::init<int32_t, int32_t>())
         .def_readwrite("x", &Coordinate::x)
         .def_readwrite("y", &Coordinate::y);

     // Vector2D
     py::class_<Vector2D>(m, "Vector2D")
         .def(py::init<int32_t, int32_t>())
         .def(py::init<>())
         .def_property_readonly("x", &Vector2D::GetX)
         .def_property_readonly("y", &Vector2D::GetY)
         .def("__add__", [](const Vector2D &a, const Vector2D &b)
              { return a + b; })
         .def("__sub__", [](const Vector2D &a, const Vector2D &b)
              { return a - b; })
         .def("__mul__", [](const Vector2D &a, int32_t v)
              { return a * v; })
         .def("__truediv__", [](const Vector2D &a, int32_t v)
              { return a / v; });

     // Entity
     py::class_<Entity>(m, "Entity")
         .def_readonly("id", &Entity::id)
         .def_readonly("type", &Entity::type)
         .def_readonly("flags", &Entity::flags)
         .def_readwrite("anchor", &Entity::anchor)
         .def_readonly("pos", &Entity::pos)
         .def_readwrite("rotation", &Entity::rotation)
         .def_readwrite("vel", &Entity::vel)
         .def("GetOccupiedTiles", &Entity::GetOccupiedTiles)
         .def("HasFlag", &Entity::HasFlag)
         .def("AddFlag", &Entity::AddFlag)
         .def("RemoveFlag", &Entity::RemoveFlag);

     // Tile
     py::class_<Tile>(m, "Tile")
         .def_readonly("localX", &Tile::localX)
         .def_readonly("localY", &Tile::localY)
         .def_readonly("solidCount", &Tile::solidCount)
         .def("GetOccupyingEntities", [](Tile &self)
              {
             py::list result;
             for (Entity* e : self.occupyingEntities) {
                 result.append(py::cast(e, py::return_value_policy::reference));
             }
             return result; });

     // Chunk
     py::class_<Chunk>(m, "Chunk")
         .def("GetX", &Chunk::GetX)
         .def("GetY", &Chunk::GetY)
         .def("GetTile", &Chunk::GetTile, py::return_value_policy::reference)
         .def("GetEntities", [](Chunk &self)
              {
             py::list result;
             const auto& ents = self.GetEntities();
             for (const auto& e : ents) {
                 result.append(py::cast(e.get(), py::return_value_policy::reference));
             }
             return result; });

     // World
     py::class_<World>(m, "World")
         .def(py::init<>())
         // Чанки
         .def("GetChunk", &World::GetChunk, py::return_value_policy::reference,
              py::arg("globalX"), py::arg("globalY"))
         .def("GetOrCreateChunk", &World::GetOrCreateChunk, py::return_value_policy::reference,
              py::arg("globalX"), py::arg("globalY"))
         .def("RemoveChunk", &World::RemoveChunk,
              py::arg("globalX"), py::arg("globalY"))

         // Сущности
         .def("SpawnEntity", [](World &self, uint32_t type, uint32_t flags, int32_t x, int32_t y, uint8_t rotation)
              { return self.SpawnEntity(type, flags, Coordinate{x, y}, rotation, nullptr); }, py::arg("type"), py::arg("flags"), py::arg("x"), py::arg("y"), py::arg("rotation") = 0)

         .def("MoveEntity", [](World &self, EntityId id, int32_t x, int32_t y)
              { return self.MoveEntity(id, Coordinate{x, y}); }, py::arg("id"), py::arg("x"), py::arg("y"))

         .def("RemoveEntity", &World::RemoveEntity, py::arg("id"))
         .def("GetEntity", &World::GetEntity, py::return_value_policy::reference, py::arg("id"))
         .def("GetAllEntities", &World::GetAllEntities, py::return_value_policy::reference_internal)

         // Движение
         .def("SetVelocity", [](World &self, EntityId id, int32_t x, int32_t y)
              { self.SetVelocity(id, Vector2D(x, y)); }, py::arg("id"), py::arg("x"), py::arg("y"))

         .def("Tick", &World::Tick, py::arg("dt"));
}
