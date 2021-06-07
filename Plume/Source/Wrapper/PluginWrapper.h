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
#include "ScanHandler.h"


class PlumeProcessor;

/**
 *  \class PluginWrapper PluginWrapper.h
 *
 *  \brief Class that manages the plugin wrapping function of plume.
 *
 *  The class holds pointers to a wrapperProcessor, an AudioPluginInstance and a WrapperEditorWindow.
 *  It can be used to wrap a plugin, unwrap it, create or destroy its editor.
 */
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
    void startScanProcess (bool dontRescanIfAlreadyInList, bool resetBlackList = false);
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
    ScanHandler& getScanner();
    void handleScanFinished();
    
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
    bool usesDefaultPaths();
    bool usesCustomPaths();
    bool usesAudioUnits();
    
private:
    //==============================================================================
    Array<File> createFileList();
    PluginDescription* getDescriptionToWrap (const PluginDescription& description);
    void loadPluginListFromFile();
    File getOrCreatePluginListFile();
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
    std::unique_ptr<WrapperProcessor> wrapperProcessor;
    std::unique_ptr<AudioPluginInstance> wrappedInstance;
    std::unique_ptr<WrapperEditorWindow> wrapperEditor;
    std::unique_ptr<AudioProcessorEditor> wrapEd;

    //==============================================================================
    ValueTree customDirectories;
    std::unique_ptr<KnownPluginList> pluginList;
    CriticalSection pluginListLock;

    std::unique_ptr<AudioPluginFormatManager> formatManager;
    std::unique_ptr<ScanHandler> scanHandler;
 
    PlumeProcessor& owner;
    GestureArray& gestArray;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWrapper)
};
