#include "action.h"

Action *action_new() {
    Action *self = calloc(1, sizeof(Action));

    return self;
}