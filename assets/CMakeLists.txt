# Copyright (C) 2025 Tom Holz.
# SPDX-License-Identifier: GPL-3.0-only

qt_add_library(acquisition_assets INTERFACE)

qt_add_resources(acquisition_assets "resources"
    BASE "assets"
    FILES
    # Fonts
    fonts/Fontin-SmallCaps.ttf
    # Icons
    icons/icon.png
    icons/icon.icns
    icons/icon.svg
    icons/icon.ico
    # Item backgrounds
    images/backgrounds/ShaperBackground_1x3.png
    images/backgrounds/ElderBackground_2x4.png
    images/backgrounds/ShaperBackground_1x1.png
    images/backgrounds/ElderBackground_2x1.png
    images/backgrounds/ShaperBackground_1x4.png
    images/backgrounds/ElderBackground_2x2.png
    images/backgrounds/ElderBackground_2x3.png
    images/backgrounds/ShaperBackground_2x3.png
    images/backgrounds/ShaperBackground_2x2.png
    images/backgrounds/ElderBackground_1x4.png
    images/backgrounds/ShaperBackground_2x1.png
    images/backgrounds/ElderBackground_1x1.png
    images/backgrounds/ShaperBackground_2x4.png
    images/backgrounds/ElderBackground_1x3.png
    # Item header images
    images/headers/ItemsHeaderRareSingleLineLeft.png
    images/headers/ItemsHeaderGemMiddle.png
    images/headers/ItemsHeaderWhiteLeft.png
    images/headers/ItemsHeaderRareLeft.png
    images/headers/ItemsHeaderGemRight.png
    images/headers/ItemsHeaderGemLeft.png
    images/headers/ItemsHeaderCurrencyMiddle.png
    images/headers/ItemsHeaderRareSingleLineMiddle.png
    images/headers/ItemsHeaderCurrencyLeft.png
    images/headers/ItemsHeaderUniqueSingleLineLeft.png
    images/headers/ItemsHeaderMagicLeft.png
    images/headers/ItemsHeaderRareMiddle.png
    images/headers/ItemsHeaderUniqueSingleLineRight.png
    images/headers/ItemsHeaderWhiteRight.png
    images/headers/ItemsHeaderUniqueLeft.png
    images/headers/ItemsHeaderRareSingleLineRight.png
    images/headers/ItemsHeaderUniqueMiddle.png
    images/headers/ItemsHeaderUniqueRight.png
    images/headers/ItemsHeaderWhiteMiddle.png
    images/headers/ItemsHeaderCurrencyRight.png
    images/headers/ItemsHeaderRareRight.png
    images/headers/ItemsHeaderMagicMiddle.png
    images/headers/ItemsHeaderUniqueSingleLineMiddle.png
    images/headers/ItemsHeaderMagicRight.png
    # Item separators
    images/separators/SeparatorUnique.png
    images/separators/SeparatorCurrency.png
    images/separators/SeparatorGem.png
    images/separators/SeparatorMagic.png
    images/separators/SeparatorWhite.png
    images/separators/SeparatorRare.png
    images/separators/SeparatorQuest.png
    # Socket images
    images/sockets/Socket_G.png
    images/sockets/Socket_S.png
    images/sockets/Socket_D.png
    images/sockets/Socket_A.png
    images/sockets/Socket_Link_Horizontal.png
    images/sockets/Socket_Link_Vertical.png
    images/sockets/Socket_I.png
    # Item symbols
    images/symbols/Synthesised-item-symbol.png
    images/symbols/ShaperItemSymbol.png
    images/symbols/Crusader-item-symbol.png
    images/symbols/Fractured-item-symbol.png
    images/symbols/ElderItemSymbol.png
    images/symbols/Eater-of-worlds-item-symbol.png
    images/symbols/Hunter-item-symbol.png
    images/symbols/Redeemer-item-symbol.png
    images/symbols/Warlord-item-symbol.png
    images/symbols/Searing-exarch-item-symbol.png
)
