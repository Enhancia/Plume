/*
  ==============================================================================

    PluginWrapper.h
    Created: 29 Jun 2018 3:03:40pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

#include "../Common/PlumeCommon.h"

#include "../Gesture/GestureArray.h"
#include "WrapperEditor.h"


/**
 *  \class PluginWrapper PluginWrapper.h
 *
 *  \brief Class that manages the plugin wrapping function of plume.
 *
 *  The class holds pointers to a wrapperProcessor, an AudioPluginInstance and a WrapperEditorWindow.
 *  It can be used to wrap a plugin, unwrap it, create or destroy its editor.
 */
class PlumeProcessor;

class PluginWrapper : public AudioProcessorParameter::Listener,
                      public ChangeBroadcaster
{
public:
    enum Formats
    {
        VST =0,
      #if JUCE_MAC
        AU,
      #endif
      #if JUCE_PLUGINHOST_VST3
        VST3,
      #endif
        numFormats
    };
   
    //==============================================================================
    PluginWrapper(PlumeProcessor&, GestureArray&, ValueTree pluginDirs);
    ~PluginWrapper();
    
    //==============================================================================
    bool wrapPlugin (PluginDescription& description);
    bool wrapPlugin (int pluginMenuId);
    void unwrapPlugin();
    bool rewrapPlugin (PluginDescription& description);
    bool rewrapPlugin (int pluginMenuId);
    bool isWrapping();
    
    //==============================================================================
    void scanAllPluginsInDirectories (bool dontRescanIfAlreadyInList, bool ignoreBlackList = false);
    PluginDirectoryScanner* getDirectoryScannerForFormat (int formatToScan);
    void savePluginListToFile();
    void removeNonInstrumentsFromList();
    void resetDeadsManPedalFile();
    
    AudioPluginFormat* getPluginFormat (File pluginFile);
    
    //==============================================================================
    void addCustomDirectory (File newDir);
    String getCustomDirectory (int numDir);
    void clearCustomDirectories();
    
    void addPluginsToMenu (PopupMenu& menu, KnownPluginList::SortMethod sort);
    KnownPluginList& getList();
    
    //==============================================================================
    void createWrapperEditor (const Component* componentWhichWindowToAttachTo);
    WrapperEditorWindow* getWrapperEditorWindow();
    void clearWrapperEditor();
    void minimiseWrapperEditor (bool shouldBeMinimised);
    void wrapperEditorToFront (bool shouldAlsoGiveFocus);
    
    //==============================================================================
    WrapperProcessor& getWrapperProcessor();
    PlumeProcessor& getOwner();
    bool hasOpenedWrapperEditor();
    
    String getWrappedPluginName();
    String getWrappedPluginInfoString();
    
    //==============================================================================
    void fillInPluginDescription (PluginDescription& pd);
    void addParametersToGestureFromXml (XmlElement& gesture, int gestureNum);
    
    //==============================================================================
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {};

    //==============================================================================
    void setDefaultPathUsage (bool shouldUseDefaultPaths);
    void setCustomPathUsage (bool shouldUseCustomPath);
    void setAuUsage (bool shouldUseAudioUnits);
    
private:
    //==============================================================================
    Array<File*> createFileList();
    PluginDescription* getDescriptionToWrap (const PluginDescription& description);
    void loadPluginListFromFile();
    File getOrCreateDeadsManPedalFile();
    
    //==============================================================================
    bool hasWrappedInstance;
    bool hasOpenedEditor;
    bool useDefaultPaths = true;
    bool useCustomPaths = true;
  #if JUCE_MAC
    bool useAudioUnits = true;
  #endif
    float scanProgress = 0.0f;
    String pluginBeingScanned;
    
    //==============================================================================
    ScopedPointer<WrapperProcessor> wrapperProcessor;
    std::unique_ptr<AudioPluginInstance> wrappedInstance;
    ScopedPointer<WrapperEditorWindow> wrapperEditor;
    ScopedPointer<AudioProcessorEditor> wrapEd;
    ScopedPointer<PlumeProgressBar> bar;

    //==============================================================================
    ValueTree customDirectories;
    ScopedPointer<KnownPluginList> pluginList;
    ScopedPointer<AudioPluginFormatManager> formatManager;
 
    PlumeProcessor& owner;
    GestureArray& gestArray;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWrapper)
};
