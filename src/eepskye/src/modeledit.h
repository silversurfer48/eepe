#ifndef MODELEDIT_H
#define MODELEDIT_H

#include <QDialog>
#include <QtGui>
#include <QPen>
#include "pers.h"
#include "mixerslist.h"

namespace Ui {
    class ModelEdit;
}

class ModelEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ModelEdit(struct t_radioData *radioData, uint8_t id, QWidget *parent = 0);
    ~ModelEdit();

    void applyBaseTemplate();
    bool redrawCurve;

    QString getNote(int i) { return mixNotes[i]; }
    void setNote(int i, QString s);
    int getModelID() { return id_model; }
    void refreshMixerList() { tabMixes(); }


private:
    struct t_radioData *rData ;
    Ui::ModelEdit *ui;
    EEPFILE *eeFile;
    class simulatorDialog *sdptr ;
    bool switchDefPosEditLock;

    MixersList *MixerlistWidget;

    QString mixNotes[MAX_SKYMIXERS];

    EEGeneral g_eeGeneral;
    SKYModelData g_model;
    int       id_model;

    bool switchEditLock;
    bool heliEditLock;
    bool protocolEditLock;
    bool plot_curve[16];
    
    QSpinBox  * cswitchOffset[NUM_SKYCSW];
    QSpinBox  * cswitchOffset0[NUM_SKYCSW];
    QComboBox * cswitchSource1[NUM_SKYCSW];
    QComboBox * cswitchSource2[NUM_SKYCSW];
    QComboBox * cswitchAndSwitch[NUM_SKYCSW];
    QLabel		* cswitchTlabel[NUM_SKYCSW];
		QTextBrowser * cswitchText1[NUM_SKYCSW];
		QTextBrowser * cswitchText2[NUM_SKYCSW];

    QSpinBox  * safetySwitchValue[NUM_SKYCHNOUT];
    QComboBox * safetySwitchSwtch[NUM_SKYCHNOUT];
		QComboBox * safetySwitchType[NUM_SKYCHNOUT];
    QComboBox * safetySwitchAlarm[NUM_SKYCHNOUT];
		QCheckBox *safetySwitchGvar[NUM_SKYCHNOUT] ;
		QComboBox *safetySwitchGindex[NUM_SKYCHNOUT] ;

    QSpinBox *expoDrSpin[4][3][2][2] ;
    QComboBox *expoDrVal[4][3][2][2] ;
		QCheckBox *expoDrGvar[4][3][2][2] ;

		QSpinBox *posb[NUM_SCALERS] ;
		QSpinBox *pmsb[NUM_SCALERS] ;
		QSpinBox *pdivsb[NUM_SCALERS] ;
		QSpinBox *pdpsb[NUM_SCALERS] ;
		QComboBox *pucb[NUM_SCALERS] ;
		QComboBox *psgncb[NUM_SCALERS] ;
		QComboBox *poffcb[NUM_SCALERS] ;
		QComboBox *psrccb[NUM_SCALERS] ;
		QLineEdit *psname[NUM_SCALERS] ;

    void setupMixerListWidget();
    void updateSettings();
    void tabModelEditSetup();
    void tabExpo();
    void tabMixes();
    void tabHeli();
    void tabLimits();
    void tabCurves();
    void tabSwitches();
    void tabSafetySwitches();
    void tabTrims();
    void tabFrsky();
    void tabTemplates();
		void tabPhase();
		void tabGvar();
    void updateCurvesTab();
    void setSwitchWidgetVisibility(int i);
		void setSafetyWidgetVisibility(int i);
    void setLimitMinMax();
    void updateSwitchesTab();
    void updateHeliTab();
		void setSafetyLabels() ;
		void updatePhaseTab() ;
		void textUpdate( QLineEdit *source, char *dest, int length ) ;
		int16_t getRawTrimValue( uint8_t phase, uint8_t idx ) ;
		uint32_t getTrimFlightPhase( uint8_t phase, uint8_t idx ) ;
		int16_t getTrimValue( uint8_t phase, uint8_t idx ) ;

    void launchSimulation();
    void resizeEvent(QResizeEvent *event  = 0);

    void setProtocolBoxes();

    void drawCurve();
    int currentCurve;
    void setCurrentCurve(int curveId);

    QSpinBox *getNodeSB(int i);

    bool gm_insertMix(int idx);
    int getMixerIndex(int dch);
    void gm_deleteMix(int index);
    void gm_openMix(int index);
    int gm_moveMix(int idx, bool dir);
    void mixersDeleteList(QList<int> list);
    QList<int> createListFromSelected();
    void setSelectedByList(QList<int> list);

    void applyTemplate(uint8_t idx);
    SKYMixData* setDest(uint8_t dch);
    void setCurve(uint8_t c, int8_t ar[]);
    void setSwitch(uint8_t idx, uint8_t func, int8_t v1, int8_t v2);


signals:
    void modelValuesChanged(ModelEdit * = 0);

private slots:
    void clearMixes(bool ask=true);
    void clearCurves(bool ask=true);

    void on_extendedLimitsChkB_toggled(bool checked);
    void on_resetCurve_1_clicked();
    void on_resetCurve_2_clicked();
    void on_resetCurve_3_clicked();
    void on_resetCurve_4_clicked();
    void on_resetCurve_5_clicked();
    void on_resetCurve_6_clicked();
    void on_resetCurve_7_clicked();
    void on_resetCurve_8_clicked();
    void on_resetCurve_9_clicked();
    void on_resetCurve_10_clicked();
    void on_resetCurve_11_clicked();
    void on_resetCurve_12_clicked();
    void on_resetCurve_13_clicked();
    void on_resetCurve_14_clicked();
    void on_resetCurve_15_clicked();
    void on_resetCurve_16_clicked();

    void mimeDropped(int index, const QMimeData *data, Qt::DropAction action);
    void pasteMIMEData(const QMimeData * mimeData, int destIdx=1000);
    void on_pushButton_clicked();
		void on_updateButton_clicked() ;
    void mixersDelete(bool ask=true);
    void mixersCut();
    void mixersCopy();
    void mixersPaste();
    void mixersDuplicate();
    void mixerOpen();
    void mixerAdd();
    void moveMixUp();
    void moveMixDown();

    void mixerlistWidget_customContextMenuRequested(QPoint pos);
    void mixerlistWidget_doubleClicked(QModelIndex index);
    void mixerlistWidget_KeyPress(QKeyEvent *event);



    void on_curveEdit_1_clicked();
    void on_curveEdit_2_clicked();
    void on_curveEdit_3_clicked();
    void on_curveEdit_4_clicked();
    void on_curveEdit_5_clicked();
    void on_curveEdit_6_clicked();
    void on_curveEdit_7_clicked();
    void on_curveEdit_8_clicked();
    void on_curveEdit_9_clicked();
    void on_curveEdit_10_clicked();
    void on_curveEdit_11_clicked();
    void on_curveEdit_12_clicked();
    void on_curveEdit_13_clicked();
    void on_curveEdit_14_clicked();
    void on_curveEdit_15_clicked();
    void on_curveEdit_16_clicked();

    void on_plotCB_1_toggled(bool checked);
    void on_plotCB_2_toggled(bool checked);
    void on_plotCB_3_toggled(bool checked);
    void on_plotCB_4_toggled(bool checked);
    void on_plotCB_5_toggled(bool checked);
    void on_plotCB_6_toggled(bool checked);
    void on_plotCB_7_toggled(bool checked);
    void on_plotCB_8_toggled(bool checked);
    void on_plotCB_9_toggled(bool checked);
    void on_plotCB_10_toggled(bool checked);
    void on_plotCB_11_toggled(bool checked);
    void on_plotCB_12_toggled(bool checked);
    void on_plotCB_13_toggled(bool checked);
    void on_plotCB_14_toggled(bool checked);
    void on_plotCB_15_toggled(bool checked);
    void on_plotCB_16_toggled(bool checked);

    void curvePointEdited();
    void limitEdited();
    void limitAuto();
    void switchesEdited();
    void safetySwitchesEdited();
    void expoEdited();
    void mixesEdited();
    void heliEdited();
    void FrSkyEdited();
		void GvarEdited() ;
		void phaseEdited() ;
    void setSwitchDefPos();

    void on_spinBox_S1_valueChanged(int value);
    void on_spinBox_S2_valueChanged(int value);
    void on_spinBox_S3_valueChanged(int value);
    void on_spinBox_S4_valueChanged(int value);

    void on_bcRUDChkB_toggled(bool checked);
    void on_bcELEChkB_toggled(bool checked);
    void on_bcTHRChkB_toggled(bool checked);
    void on_bcAILChkB_toggled(bool checked);
    void on_bcP1ChkB_toggled(bool checked);
    void on_bcP2ChkB_toggled(bool checked);
    void on_bcP3ChkB_toggled(bool checked);

    void on_thrExpoChkB_toggled(bool checked);
    void on_thrTrimChkB_toggled(bool checked);
    void on_TrainerChkB_toggled(bool checked);
//    void on_T2ThrTrgChkB_toggled(bool checked);
		
    void on_ppmDelaySB_editingFinished();
    void on_numChannelsSB_editingFinished();
		void on_numChannels2SB_editingFinished() ;
		void on_startChannelsSB_editingFinished() ;
		void on_startChannels2SB_editingFinished() ;
    void on_timerValTE_editingFinished();
    void on_timer2ValTE_editingFinished();
    void on_protocolCB_currentIndexChanged(int index);
    void on_pulsePolCB_currentIndexChanged(int index);
    void on_trimSWCB_currentIndexChanged(int index);
    void on_trimIncCB_currentIndexChanged(int index);
    void on_volumeControlCB_currentIndexChanged(int index) ;
    void on_timerDirCB_currentIndexChanged(int index);
    void on_timerModeCB_currentIndexChanged(int index);
    void on_timerModeBCB_currentIndexChanged(int index);
    void on_timer2DirCB_currentIndexChanged(int index);
    void on_timer2ModeCB_currentIndexChanged(int index);
    void on_timer2ModeBCB_currentIndexChanged(int index);
    void on_modelNameLE_editingFinished();
    void on_tabWidget_currentChanged(int index);
    void on_templateList_doubleClicked(QModelIndex index);
    void on_ppmFrameLengthDSB_editingFinished();
    void ControlCurveSignal(bool flag);
    void on_DSM_Type_currentIndexChanged(int index);
    void on_pxxRxNum_editingFinished();
		void on_VoiceNumberSB_editingFinished() ;
		void on_autoLimitsSB_editingFinished() ;
		void on_countryCB_currentIndexChanged(int index) ;
		void on_typeCB_currentIndexChanged(int index) ;
		void updateToMV2( void ) ;

		void on_switchwarnChkB_stateChanged(int ) ;
		void getModelSwitchDefPos(int i, bool val) ;
    void on_switchDefPos_1_stateChanged(int );
    void on_switchDefPos_2_stateChanged(int );
    void on_switchDefPos_3_stateChanged(int );
    void on_switchDefPos_4_stateChanged(int );
    void on_switchDefPos_5_stateChanged(int );
    void on_switchDefPos_6_stateChanged(int );
    void on_switchDefPos_7_stateChanged(int );
    void on_switchDefPos_8_stateChanged(int );

};



#endif // MODELEDIT_H

