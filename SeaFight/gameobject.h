#pragma once

#include "sprite.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

struct Transform2dComponent {
  glm::vec2 translation{};  
  glm::vec2 scale{1.f, 1.f};
  float rotation;

  glm::mat4 mat4() {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(translation, 0.0f));
      model = glm::translate(model, glm::vec3(0.5f * scale.x, 0.5f * scale.y, 0.0f));
      model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
      model = glm::translate(model, glm::vec3(-0.5f * scale.x, -0.5f * scale.y, 0.0f));

      model = glm::scale(model, glm::vec3(scale, 1.0f));
      return model;
  }
};

class GameObject {
 public:
  using id_t = unsigned int;

  static GameObject createGameObject() {
    static id_t currentId = 0;
    return GameObject{currentId++};
  }

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;
  GameObject(GameObject &&) = default;
  GameObject &operator=(GameObject &&) = default;

  id_t getId() { return id; }

  std::shared_ptr<Sprite> sprite{};
  glm::vec3 color{};
  Transform2dComponent transform2d{};

 private:
  GameObject(id_t objId) : id{objId} {}

  id_t id;
};
