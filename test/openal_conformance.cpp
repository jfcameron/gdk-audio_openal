// © Joseph Cameron - All Rights Reserved

#include "conformance.h"

#include <gdk/audio/openal_context.h>

namespace gdk::audio::conformance {
    context_shared_ptr_type make_context() { return openal_context::make(); }

    std::string_view implementation() { return "openal"; }
}
