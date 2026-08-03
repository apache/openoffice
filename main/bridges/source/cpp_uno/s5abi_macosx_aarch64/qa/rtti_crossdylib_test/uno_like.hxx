#pragma once
namespace com { namespace sun { namespace star {
namespace uno { struct Exception { char* Message; void* Context; }; }
namespace ucb { struct InteractiveAugmentedIOException : public ::com::sun::star::uno::Exception { int code; }; }
}}}
