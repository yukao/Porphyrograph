#ifndef PGINTERFACESONGDATA_H
#define PGINTERFACESONGDATA_H

#include <vector>

#include <QGroupBox>
#include <QRect>
#include <QLabel>
#include <QColor>
#include <QString>

///////////////////////////////////////////////////////////////
/// \brief The inControllerData class
///
class ControllerData {
public:
    ControllerData(
            int typeIn_par,
            int controlsIn_par,
            int channelsIn_par,
            int controlsOut_par, // added to In
            QString labelUDPIn_par,
            QString labelsHighlight_par,
            QString labelsUDPOut_par, // added to In
            float amplitude_par,
            QString titleDisplay_par, // added to In
            QString titleController_par,
            QString labelObject_par,
            QString contolKey_par, // added to In
            float pulseControl_par, // whether or not the parameter can be pulsed
            int size_par,
            int intervalle_par,
            int fontSize_par
            );
    ~ControllerData();

    QColor bgOffColor;
    QColor bgOnColor;
    QColor barNormalColor;
    QColor barOverflowColor;
    QColor barUnderflowColor;
    int size;
    int intervalle;
    int fontSize;

    // UDP messages
    QString get_labelUDPIn();
    QString get_labelHighlightIn();
    QString  get_labelUDPOut(); // added to In
    // MIDI in
    int  get_typeMidiIn();
    int  get_controlMidiIn();
    int  get_channelMidiIn();
    int  get_controlMidiOut(); // added to In
    float  get_amplitude();
    int  get_size();
    // GUI names: displayed name - name used for styling
    QString  get_titleDisplay();
    QString  get_titleController();
    QString  get_labelObject();
    // control Key (for buttons)
    QString get_contolKey(); // added to In
    float getPulseControlFactor(void);
private:
    // UDP messages
    QString labelUDPIn;
    QString labelHighlightIn;
    QString labelUDPOut; // added to In
    // MIDI in
    int typeMidiIn;
    int controlMidiIn;
    int channelMidiIn;
    int controlMidiOut; // added to In // midi out for LED
    float amplitude;
    // GUI names: displayed name - name used for styling
    QString titleDisplay;
    QString titleController;
    QString labelObject;
    // control Key (for buttons)
    QString contolKey; // added to In
    float pulseControlFactor; // whether or not the parameter can be pulsed
};

enum DisplayType { _Config_display = 0, _Beat_display, _Beat_Pulse_display, _Mixer_display, _Mixer_Pulse_display,
                   _FX_display, _FX_Pulse_display, _Paths_display, _Paths_Pulse_display,
#if defined(TVW) || defined(demo)
                   _MinimalIF_display, _MinimalIF_Pulse_display,
#endif
                   _NumberOfDisplays };

enum SlidersBeatDataIDs { _FlashGridRowCASlidersBeatData = 0, _FlashGridRowPartSlidersBeatData, _FlashGridRowBGSlidersBeatData,
                            _PixelAccCentSlidersBeatData, _BeatSoundBandsSlidersBeatData,
                            _PixelAccCoefSlidersBeatData, _XXXXSlidersBeatData,
                            _RadiusSlidersBeatData,
                            _ColorSlidersBeatData, _MasterBlendSlidersBeatData,
                            _MaxSlidersBeatData };
enum ButtonsBeatDataIDs { _penBWButtonsBeatData = 0, _penPaletteButtonsBeatData, _pixelModeButtonsBeatData,
                            _repopBWButtonsBeatData, _penBrushButtonsBeatData, _FlashVideoButtonsBeatData, _sideButtonsBeatData,
                            _MaxButtonsBeatData };

enum SlidersMixerDataIDs { _MixingSlidersMixerData = 0, _MasterSlidersMixerData,
                            _CameraSlidersMixerData,
                            _MovieMediaFreqSlidersMixerData,
                            _MixerSoundBandsSlidersMixerData,
                            _RepopSlidersMixerData, _XXXXSlidersMixerData,_PhotoSlidersMixerData,
                            _DecaySlidersMixerData, _EchoSlidersMixerData,
                            _MasterBlendSlidersMixerData, _MaxSlidersMixerData };
enum ButtonsMixerDataIDs { _TracksButtonsMixerData = 0, _TrackCopyButtonsMixerData,
                            _decaySignButtonMixersData, _freezeBottomButtonsMixerData, _sideButtonsMixerData,
                            _MaxButtonsMixerData };

enum SlidersFXDataIDs { _CAParamsSlidersFXData = 0,
                        _CameraParamsSlidersFXData,
                        _FXSoundBandsSlidersFXData,
                        _noiseSlidersFXData, _VideoSatFXData,
                        _MediaSobelSlidersFXData, _BeatSlidersFXData,
                        _SoundSlidersFXData, _SoundVolumeSlidersFXData, _MaxSlidersFXData };
enum ButtonsFXDataIDs { _CATypeButtonFXsData = 0, _mediaPlusButtonsFXData, _cameraCumulButtonsFXData,
                        _sensorsButtonsFXData, _noiseTypeButtonFXsData, _micOnOffButtonsFXData, _soundSampleButtonsFXData,
                        _sideButtonsFXData,
                        _MaxButtonsFXData };

enum SlidersPathsDataIDs { _ParticleRadiusSlidersPathsData = 0, _ParticleFluiditySlidersPathsData,
                           _ParticleNoiseSlidersPathsData, _MaxSlidersPathsData };
enum ButtonsPathsDataIDs { _partMoveButtonsPathsData = 0, _partStrokeButtonsPathsData, _partExitButtonsPathsData,
                           _partColorButtonsPathsData, _partInitButtonsPathsData, _partRepopPathButtonsPathsData,
                           _partFollowPathButtonsPathsData, _partRepulsePathButtonsPathsData, _pathRecordButtonsPathsData,
                           _pathReplayButtonsPathsData, _setUpButtonsPathsData, _clearButtonsPathsData, _sideButtonsPathsData,
                           _MaxButtonsPathsData };

#if defined(TVW) || defined(demo)
enum SlidersMinimalIFDataIDs { _EchoMinimalIFData = 0, _MixingMinimalIFData,
                         _PenRepopMinimalIFData, _MasterMinimalIFData,
                         _FXSoundBandsSlidersMinimalIFData,
                         _PixelsMinimalIFData, _FlashMinimalIFData,
                         _PenSlidersMinimalIFData, _DecaySlidersMinimalIFData,
                         _CameraSlidersMinimalIFData, _SoundSlidersMinimalIFData, _MasterBlendSlidersMinimalIFData, _MaxSlidersMinimalIFData };
enum ButtonsMinimalIFDataIDs { _PenColorMinimalIFData = 0, _PixeCAlMinimalIFData, _SampleMinimalIFData,
                         _PenGreyButtonTVWsData, _DecayButtonTVWsData,
#if defined(TVW)
                         _flashCameraMinimalIFData,
#endif
#if defined(demo)
                         _trackBrushMinimalIFData,
#endif
                         _sideButtonsMinimalIFData,
                         _MaxButtonsMinimalIFData };
#endif

extern std::vector<std::vector<ControllerData>> SlidersBeatData;
extern std::vector<QString> SlidersBeatData_verticalSlidersRowTitles;
extern std::vector<QString> SlidersBeatData_LEDBarRowTitles;
extern std::vector<std::vector<ControllerData>> ButtonsBeatData;
extern std::vector<QString> ButtonsBeatData_ButtonTitles;
extern std::vector<QString> ButtonsBeatData_LabelTitles;

extern std::vector<std::vector<ControllerData>> SlidersMixerData;
extern std::vector<QString> SlidersMixerData_verticalSlidersRowTitles;
extern std::vector<QString> SlidersMixerData_LEDBarRowTitles;
extern std::vector<std::vector<ControllerData>> ButtonsMixerData;
extern std::vector<QString> ButtonsMixerData_ButtonTitles;
extern std::vector<QString> ButtonsMixerData_LabelTitles;

extern std::vector<std::vector<ControllerData>> SlidersFXData;
extern std::vector<QString> SlidersFXData_verticalSlidersRowTitles;
extern std::vector<QString> SlidersFXData_LEDBarRowTitles;
extern std::vector<std::vector<ControllerData>> ButtonsFXData;
extern std::vector<QString> ButtonsFXData_ButtonTitles;
extern std::vector<QString> ButtonsFXData_LabelTitles;

extern std::vector<std::vector<ControllerData>> SlidersPathsData;
extern std::vector<QString> SlidersPathsData_verticalSlidersRowTitles;
extern std::vector<QString> SlidersPathsData_LEDBarRowTitles;
extern std::vector<std::vector<ControllerData>> ButtonsPathsData;
extern std::vector<QString> ButtonsPathsData_ButtonTitles;
extern std::vector<QString> ButtonsPathsData_LabelTitles;

#if defined(TVW) || defined(demo)
extern std::vector<std::vector<ControllerData>> SlidersMinimalIFData;
extern std::vector<QString> SlidersMinimalIFData_verticalSlidersRowTitles;
extern std::vector<QString> SlidersMinimalIFData_LEDBarRowTitles;
extern std::vector<std::vector<ControllerData>> ButtonsMinimalIFData;
extern std::vector<QString> ButtonsMinimalIFData_ButtonTitles;
extern std::vector<QString> ButtonsMinimalIFData_LabelTitles;
#endif

extern std::vector<QString> TabbedWindowTitles;

#endif // PGINTERFACESONGDATA_H