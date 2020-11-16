/*
  ==============================================================================

    WrappedPluginProcessor.cpp
    Created: 9 Apr 2018 3:23:04pm
    Author:  Alex LEVACHER

  ==============================================================================
*/

#include "PluginWrapper.h"

//==============================================================================
/*class that wraps a parameter from the plugin, forwarding it's methods to the wrapperProcessor*/

class WrapperProcessor::WrappedParameter :  public AudioProcessorParameter
{
public:
    explicit WrappedParameter(AudioProcessorParameter& wrap)
        : wrappedParam(wrap)
    {}
    
    float getValue() const override                          { return wrappedParam.getValue();         }
    void setValue (float newValue) override                  {        wrappedParam.setValue (newValue);}
    float getDefaultValue() const override                   { return wrappedParam.getDefaultValue();  }
    String getName (int maxLen) const override               { return wrappedParam.getName (maxLen);   }
    String getLabel() const override                         { return wrappedParam.getLabel();         }
    int getNumSteps() const override                         { return wrappedParam.getNumSteps();      }
    bool isDiscrete() const override                         { return wrappedParam.isDiscrete();       }
        
    String getText (float v, int len) const override         { return wrappedParam.getText (v, len);        }
    float getValueForText (const String& t) const override   { return wrappedParam.getValueForText (t);     }
    String getCurrentValueAsText() const override            { return wrappedParam.getCurrentValueAsText(); }
    bool isOrientationInverted() const override              { return wrappedParam.isOrientationInverted(); }
    bool isAutomatable() const override                      { return wrappedParam.isAutomatable();         }
    bool isMetaParameter() const override                    { return wrappedParam.isMetaParameter();       }
    Category getCategory() const override                    { return wrappedParam.getCategory();           }
    
private:
    AudioProcessorParameter& wrappedParam;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WrappedParameter)
};


//==============================================================================
WrapperProcessor::WrapperProcessor(AudioPluginInstance& wrappedPlugin, PluginWrapper& ownerWrapper)
    : AudioProcessor (BusesProperties()
                       .withInput ("Input", AudioChannelSet::stereo(), true)
                       .withOutput ("Output", AudioChannelSet::stereo(), true)),
      plugin (wrappedPlugin),
      owner (ownerWrapper)
{
    TRACE_IN;
    initWrappedParameters();
}

WrapperProcessor::~WrapperProcessor()
{
    TRACE_IN;
    auto& params = plugin.getParameters();
    
    for (auto* param : params)
    {
        param->removeListener (&getOwnerWrapper());
    }
	TRACE_OUT;
}

//==============================================================================
void WrapperProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    // Clears the audioBuffer if its not needed
    if (plugin.getMainBusNumInputChannels() == 0)
    {
        buffer.clear();
    }
    
    // Makes the plugin use playhead from the DAW
    plugin.setPlayHead (getPlayHead());

    plugin.processBlock (buffer, midiMessages);
}


//==============================================================================
void WrapperProcessor::initWrappedParameters()
{
    auto& params = plugin.getParameters();
    
    for (auto* param : params)
    {
        addParameter(new WrappedParameter(*param));
        param->addListener (&getOwnerWrapper());
    }
}

AudioProcessorParameter& WrapperProcessor::getWrappedParameter (int id)
{
    TRACE_IN;
    auto& params = plugin.getParameters();
    
    return *(params[id]);
}

PluginWrapper& WrapperProcessor::getOwnerWrapper()
{
    return owner;
}

AudioPluginInstance& WrapperProcessor::getWrappedInstance()
{
    return plugin;
}

void WrapperProcessor::clearEditor()
{
    owner.clearWrapperEditor();
}