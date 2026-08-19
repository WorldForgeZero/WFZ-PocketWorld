// _core.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "chunk.h"
#include "coordinate.h"
#include "entity.h"
#include "resistance.h"
#include "tile.h"
#include "vector_2d.h"
#include "world.h"

namespace py = pybind11;

PYBIND11_MODULE(_core, m)
{
     m.doc() = "WorldForgeZero PocketWorld Engine (C extension).";

     m.attr("ENTITY_SOLID") = static_cast<uint32_t>(ENTITY_SOLID);
     m.attr("ENTITY_FLOOR") = static_cast<uint32_t>(ENTITY_FLOOR);
     m.attr("ENTITY_EPHEMERAL") = static_cast<uint32_t>(ENTITY_EPHEMERAL);

     py::class_<Resistance>(m, "Resistance")
         .def(py::init<>())
         .def_readwrite("gas", &Resistance::gas)
         .def_readwrite("mana", &Resistance::mana)
         .def_readwrite("rad", &Resistance::rad)
         .def_readwrite("light", &Resistance::light);

     py::class_<Coordinate>(m, "Coordinate")
         .def(py::init<int32_t, int32_t>())
         .def_readwrite("x", &Coordinate::x)
         .def_readwrite("y", &Coordinate::y);

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

     py::class_<Entity>(m, "Entity")
         .def_readonly("id", &Entity::id)
         .def_readonly("type", &Entity::type)
         .def_readonly("flags", &Entity::flags)
         .def_readwrite("anchor", &Entity::anchor)
         .def_readonly("pos", &Entity::pos)
         .def_readwrite("rotation", &Entity::rotation)
         .def_readonly("res", &Entity::res, py::return_value_policy::reference)
         .def("GetOccupiedTiles", &Entity::GetOccupiedTiles)
         .def("HasFlag", &Entity::HasFlag)
         .def("AddFlag", &Entity::AddFlag)
         .def("RemoveFlag", &Entity::RemoveFlag);

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

     py::class_<World>(m, "World")
         .def(py::init<>())

         // Чанки
         .def("GetChunk", &World::GetChunk, py::return_value_policy::reference,
              py::arg("globalX"), py::arg("globalY"))
         .def("GetOrCreateChunk", &World::GetOrCreateChunk, py::return_value_policy::reference,
              py::arg("globalX"), py::arg("globalY"))
         .def("RemoveChunk", &World::RemoveChunk,
              py::arg("globalX"), py::arg("globalY"))

         // Тайл
         .def("GetTile", &World::GetTile, py::return_value_policy::reference,
              py::arg("globalX"), py::arg("globalY"))

         // Сущности
         .def("SpawnEntity", [](World &self, uint32_t type, uint32_t flags, int32_t x, int32_t y, uint8_t rotation)
              { return self.SpawnEntity(type, flags, Coordinate{x, y}, rotation, nullptr); }, py::arg("type"), py::arg("flags"), py::arg("x"), py::arg("y"), py::arg("rotation") = 0)

         .def("MoveEntity", [](World &self, EntityId id, int32_t x, int32_t y)
              { return self.MoveEntity(id, Coordinate{x, y}); }, py::arg("id"), py::arg("x"), py::arg("y"))

         .def("RemoveEntity", &World::RemoveEntity, py::arg("id"))
         .def("GetEntity", &World::GetEntity, py::return_value_policy::reference, py::arg("id"))

         // GetAllEntities оборачиваем, чтобы не было передачи владения
         .def("GetAllEntities", [](World &self)
              {
            const auto& entities = self.GetAllEntities();
            py::list result;
            for (Entity* e : entities) {
                result.append(py::cast(e, py::return_value_policy::reference));
            }
            return result; })

         .def("SetVelocity", &World::SetVelocity, py::arg("id"), py::arg("x"), py::arg("y"))

         .def("Tick", &World::Tick, py::arg("dt"))

         // Поиск: оборачиваем, чтобы не передавать владение сущностями
         .def("GetEntitiesInRect", [](World &self, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, uint32_t flags)
              {
            auto entities = self.GetEntitiesInRect(minX, minY, maxX, maxY, flags);
            py::list result;
            for (Entity* e : entities) {
                result.append(py::cast(e, py::return_value_policy::reference));
            }
            return result; }, py::arg("minX"), py::arg("minY"), py::arg("maxX"), py::arg("maxY"), py::arg("flags") = 0)

         .def("GetEntitiesInRadius", [](World &self, int32_t centerX, int32_t centerY, int32_t radius, uint32_t flags)
              {
            auto entities = self.GetEntitiesInRadius(Coordinate{centerX, centerY}, radius, flags);
            py::list result;
            for (Entity* e : entities) {
                result.append(py::cast(e, py::return_value_policy::reference));
            }
            return result; }, py::arg("centerX"), py::arg("centerY"), py::arg("radius"), py::arg("flags") = 0)

         .def("GetEntitiesInSquare", [](World &self, int32_t centerX, int32_t centerY, int32_t halfSize, uint32_t flags)
              {
            auto entities = self.GetEntitiesInSquare(Coordinate{centerX, centerY}, halfSize, flags);
            py::list result;
            for (Entity* e : entities) {
                result.append(py::cast(e, py::return_value_policy::reference));
            }
            return result; }, py::arg("centerX"), py::arg("centerY"), py::arg("halfSize"), py::arg("flags") = 0)

         .def("RaycastFirst", [](World &self, int32_t fromX, int32_t fromY, int32_t toX, int32_t toY, uint32_t flags) -> Entity *
              { return self.RaycastFirst(Coordinate{fromX, fromY}, Coordinate{toX, toY}, flags); }, py::arg("fromX"), py::arg("fromY"), py::arg("toX"), py::arg("toY"), py::arg("flags"), py::return_value_policy::reference);
}
