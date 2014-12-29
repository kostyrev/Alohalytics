#ifndef EVENT_BASE_H
#define EVENT_BASE_H

// Define ALOHALYTICS_SERVER when using this header on a server-side.

#include <string>
#include <chrono>

#include "cereal/include/cereal.hpp"
#include "cereal/include/types/base_class.hpp"
#include "cereal/include/types/polymorphic.hpp"

// For easier processing on a server side, every statistics event should derive from this base class.
struct AlohalyticsBaseEvent {
  uint64_t timestamp;

  static uint64_t CurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
  }

  // To avoid unnecessary calls on a server side
#ifndef ALOHALYTICS_SERVER
  AlohalyticsBaseEvent() : timestamp(CurrentTimestamp()) {}
#endif

  template <class Archive> void serialize(Archive& ar) {
    ar(CEREAL_NVP(timestamp));
  }

  // To use polymorphism on a server side.
  virtual ~AlohalyticsBaseEvent() = default;
};
CEREAL_REGISTER_TYPE_WITH_NAME(AlohalyticsBaseEvent, "b");

// Special event in the beginning of each block (file) sent to stats server.
// Designed to mark all events in this data block as belonging to one user with specified id.
struct AlohalyticsIdEvent : public AlohalyticsBaseEvent {
  std::string id;

  template <class Archive> void serialize(Archive& ar) {
    AlohalyticsBaseEvent::serialize(ar);
    ar(CEREAL_NVP(id));
  }
};
CEREAL_REGISTER_TYPE_WITH_NAME(AlohalyticsIdEvent, "i");

// Simple event with a string name (key) only.
struct AlohalyticsKeyEvent : public AlohalyticsBaseEvent {
  std::string key;

  template <class Archive> void serialize(Archive& ar) {
    AlohalyticsBaseEvent::serialize(ar);
    ar(CEREAL_NVP(key));
  }
};
CEREAL_REGISTER_TYPE_WITH_NAME(AlohalyticsKeyEvent, "k");

// Simple event with a string key/value pair.
struct AlohalyticsKeyValueEvent : public AlohalyticsKeyEvent {
  std::string value;

  template <class Archive> void serialize(Archive& ar) {
    AlohalyticsKeyEvent::serialize(ar);
    ar(CEREAL_NVP(value));
  }
};
CEREAL_REGISTER_TYPE_WITH_NAME(AlohalyticsKeyValueEvent, "v");

// Simple event with a string key and map<string, string> value.
struct AlohalyticsKeyPairsEvent : public AlohalyticsKeyEvent {
  std::map<std::string, std::string> pairs;

  template <class Archive> void serialize(Archive& ar) {
    AlohalyticsKeyEvent::serialize(ar);
    ar(CEREAL_NVP(pairs));
  }
};
CEREAL_REGISTER_TYPE_WITH_NAME(AlohalyticsKeyPairsEvent, "p");

#endif  // EVENT_BASE_H
