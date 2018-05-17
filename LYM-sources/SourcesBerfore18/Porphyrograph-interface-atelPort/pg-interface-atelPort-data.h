#ifndef PGINTERFACESONGDATA_H
#define PGINTERFACESONGDATA_H

#define ATELIERS_PORTATIFS
//#define GENERATIVE_NIGHTS

#include <vector>

#include <QGroupBox>
#include <QRect>
#include <QLabel>
#include <QColor>
#include <QString>

/*
///////////////////////////////////////////////////////////////
/// \brief The inControllerData class
///
class inControllerData {
public:
    inControllerData(
            int typeIn_par,
            int controlsIn_par,
            int channelsIn_par,
            QString labelUDPIn_par,
            QString labelsHighlight_par,
            float amplitude_par,
            QString label_par,
            QString labelObject_par,
            float rBgOff_par, float gBgOff_par, float bBgOff_par,
            float rBgOn_par, float gBgOn_par, float bBgOn_par,
            float rBarOff_par, float gBarOff_par, float bBarOff_par,
            float rBarOn_par, float gBarOn_par, float bBarOn_par,
            int size_par,
            int intervalle_par,
            int fontSize_par
            );
    ~inControllerData();

    QColor bgOffColor;
    QColor bgOnColor;
    QColor barOffColor;
    QColor barOnColor;
    int size;
    int intervalle;
    int fontSize;

    // UDP messages
    QString get_labelUDPIn();
    QString get_labelHighlightIn();
    // MIDI in
    int  get_typeMidiIn();
    int  get_controlMidiIn();
    int  get_channelMidiIn();
    float  get_amplitude();
    int  get_size();
    // GUI names: displayed name - name used for styling
    QString  get_title();
    QString  get_labelobject_names();
private:
    // UDP messages
    QString labelUDPIn;
    QString labelHighlightIn;
    // MIDI in
    int typeMidiIn;
    int controlMidiIn;
    int channelMidiIn;
    float amplitude;
    // GUI names: displayed name - name used for styling
    QString title;
    QString labelobject_names;
};

*/

///////////////////////////////////////////////////////////////
/// \brief The outControllerData class
///
class outControllerData {
public:
    outControllerData(
            int typeIn_par,
            int controlsIn_par,
            int channelsIn_par,
            int controlsOut_par,
            QString labelsUDPIn_par,
            QString labelsHighlight_par,
            QString labelsUDPOut_par,
            float amplitude_par,
            QString title_par,
            QString labelobject_names_par,
            QString contolKey_par
            );
    ~outControllerData();
    // UDP messages
    QString  get_labelUDPIn();
    QString  get_labelHighlightIn();
    QString  get_labelUDPOut();
    // MIDI in/out
    int  get_typeMidiIn();
    int  get_controlMidiIn();
    int  get_channelMidiIn();
    int  get_controlMidiOut();
    float  get_amplitude();
    // GUI names: displayed name - name used for styling
    QString  get_title();
    QString  get_labelobject_names();
    // control Key (for buttons)
    QString get_contolKey();
private:
    // UDP messages
    QString labelUDPIn;
    QString labelHighlightIn;
    QString labelUDPOut;
    // MIDI in/out
    int typeMidiIn;
    int controlMidiIn;
    int channelMidiIn;
    int controlMidiOut; // midi out for LED
    float amplitude;
    // GUI names: displayed name - name used for styling
    QString title;
    QString labelobject_names;
    // control Key (for buttons)
    QString contolKey;
};

///////////////////////////////////////////////////////////////
/// \brief The inControllerData class
///
class inOutControllerData {
public:
    inOutControllerData(
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
            float rBgOff_par, float gBgOff_par, float bBgOff_par,
            float rBgOn_par, float gBgOn_par, float bBgOn_par,
            float rBarOff_par, float gBarOff_par, float bBarOff_par,
            float rBarOn_par, float gBarOn_par, float bBarOn_par,
            int size_par,
            int intervalle_par,
            int fontSize_par
            );
    ~inOutControllerData();

    QColor bgOffColor;
    QColor bgOnColor;
    QColor barOffColor;
    QColor barOnColor;
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
};

enum GraphsInOutData { _FlashGridRowCAInOutData = 0, _FlashGridRowPartInOutData, _FlashGridRowBGInOutData,
                       _PixelInOutData, _PulseCAFlashInOutData, _GraphsSoundBandsInOutData,
                       _PulsePartInOutData, _CAParamsInOutData,
                       _RadiusColorInOutData,
                       _BeatInOutData, _PixRadiusInOutData, _MaxGraphsInOutDataRows };
enum TracksInInOutData { _MixingInOutData = 0, _MediaWeightInOutData, _TracksSoundBandsInOutData,
                         _MasterInOutData, _CamThresholdInOutData,  _MediaFreqInOutData,
                         _RepopInOutData, _SoundMinInOutData, _MediaSobelInOutData,
                         _DecayInOutData, _SoundVolInOutData, _EchoInOutData, _MasterBlendInOutData, _MaxTracksInOutDataRows };
enum PathsInInOutData { _ParticleRadiusInOutData = 0, _ParticleFluidityInOutData,
                        _ParticleNoiseInOutData, _MaxPathsInOutDataRows };

extern std::vector<std::vector<inOutControllerData>> graphs_mainSlidersData;
extern std::vector<std::vector<inOutControllerData>> tracks_mainSlidersData;
extern std::vector<std::vector<inOutControllerData>> paths_mainSlidersData;
extern std::vector<std::vector<inOutControllerData>> graphs_mainButtonsData;
extern std::vector<std::vector<inOutControllerData>> tracks_mainButtonsData;
extern std::vector<std::vector<inOutControllerData>> paths_mainButtonsData;
extern std::vector<inOutControllerData> graphs_sideButtonsData;
extern std::vector<inOutControllerData> tracks_sideButtonsData;



#endif // PGINTERFACESONGDATA_H
