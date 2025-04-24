#ifndef UIACTIONSENUM_H
#define UIACTIONSENUM_H

enum class UiActionsEnum {
    CANCEL = 0,
    PLAY_CARD = 1,
    ATTACK = 2,
    SHOW_BOARD = 3,
    SETTINGS = 4,
    END_TURN = 5,
};

inline int uiActionsEnumToInt(UiActionsEnum uiActionsEnum) {
    switch (uiActionsEnum) {
        case UiActionsEnum::PLAY_CARD: return 1;
        case UiActionsEnum::ATTACK: return 2;
        case UiActionsEnum::SHOW_BOARD: return 3;
        case UiActionsEnum::SETTINGS: return 4;
        case UiActionsEnum::END_TURN: return 5;
        case UiActionsEnum::CANCEL: ;
        default: return 0;
    }
}

#endif
