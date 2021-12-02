#pragma once
namespace NsModePresenter {
enum class TickReturnCode:uint8_t {
    noOp = 0,
    presentationInProgress = 1,
    presentationFinished = 2,
    presentationPending = 3
};

enum class State:uint8_t {
    noOp,
    presentationInProgress,
    presentationPending
};

}
