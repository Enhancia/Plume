/*
  ==============================================================================

    PlumeParameter.h
    Created: 9 Feb 2022 4:19:39pm
    Author:  alexl

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

template<typename Parameter>
class PlumeParameter : public Parameter
{
public:
    template<typename... Args>
    PlumeParameter (Args... args) : Parameter (std::forward<Args> (args)...)
    {
        name = Parameter::getName (100);
    }

    juce::String getName (int maximumStringLength) const override
    {
        return name.substring (0, maximumStringLength);
    }

    void setName (String newName)
    {
        name = newName;
    }

private:
    juce::String name;
};