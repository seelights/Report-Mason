//========================================================================
//
// AsyncObject.cc
//
//========================================================================

#include "poppler-form.h"

namespace Poppler {

// Forward declaration of private class
class AsyncObjectPrivate
{
public:
    AsyncObjectPrivate() = default;
    ~AsyncObjectPrivate() = default;
};

AsyncObject::AsyncObject() : QObject(nullptr), d(std::make_unique<AsyncObjectPrivate>())
{
}

AsyncObject::~AsyncObject() = default;

void AsyncObject::done()
{
    // This method should be called by external code to signal completion
    // The actual signal emission is handled by Qt's signal system
    // Note: This is a stub implementation - the actual signal emission
    // should be handled by the calling code, not here
}

} // namespace Poppler
