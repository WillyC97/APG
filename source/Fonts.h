#pragma once

#include "FontsPoppins.h"

#include <juce_gui_extra/juce_gui_extra.h>

namespace APG
{
namespace internal
{
    namespace Fonts
    {

        enum Type
        {
            Regular = 0,
            SemiBold,
            Bold,
            Italic,
            Thin
        };

        struct APGFonts
        {
            juce::Array<juce::Font> fonts;

            APGFonts()
            {
                fonts.set(Type::Regular,
                          juce::Font(juce::Typeface::createSystemTypefaceFor(FontsPoppins::PoppinsRegular_ttf, FontsPoppins::PoppinsRegular_ttfSize))
                );
                
                fonts.set(Type::SemiBold,
                          juce::Font(juce::Typeface::createSystemTypefaceFor(FontsPoppins::PoppinsSemiBold_ttf, FontsPoppins::PoppinsSemiBold_ttfSize))
                );

                fonts.set(Type::Bold,
                          juce::Font(juce::Typeface::createSystemTypefaceFor(FontsPoppins::PoppinsBold_ttf, FontsPoppins::PoppinsBold_ttfSize))
                );

                fonts.set(Type::Italic,
                          juce::Font(juce::Typeface::createSystemTypefaceFor(FontsPoppins::PoppinsItalic_ttf, FontsPoppins::PoppinsItalic_ttfSize))
                );
                
                fonts.set(Type::Thin,
                          juce::Font(juce::Typeface::createSystemTypefaceFor(FontsPoppins::PoppinsThin_ttf, FontsPoppins::PoppinsThin_ttfSize))
                );
            }

            juce::Font GetFont(Type fontId, float height)
            {
                return fonts[fontId].withHeight(height);
            }
        };

        static juce::Font GetFont(Type fontId, float height)
        {
            static APGFonts apgFonts;
            return apgFonts.GetFont(fontId, height);
        }
    }
}
}
