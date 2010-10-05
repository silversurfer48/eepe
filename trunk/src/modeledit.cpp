#include "modeledit.h"
#include "ui_modeledit.h"
#include "pers.h"
#include "helpers.h"
#include "edge.h"
#include "node.h"

#include <QtGui>

#define BC_BIT_RUD (0x01)
#define BC_BIT_ELE (0x02)
#define BC_BIT_THR (0x04)
#define BC_BIT_AIL (0x08)
#define BC_BIT_P1  (0x10)
#define BC_BIT_P2  (0x20)
#define BC_BIT_P3  (0x40)

#define RUD  (1)
#define ELE  (2)
#define THR  (3)
#define AIL  (4)

#define SRC_STR "----RUD ELE THR AIL RUD P1  P2  P3  MAX FULLPPM1PPM2PPM3PPM4PPM5PPM6PPM7PPM8CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8 CH9 CH10CH11CH12CH13CH14CH15CH16CH17CH18CH19CH20CH21CH22CH23CH24CH25CH26CH27CH28CH29CH30";

ModelEdit::ModelEdit(EEPFILE *eFile, uint8_t id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelEdit)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));

    eeFile = eFile;

    if(!eeFile->eeLoadGeneral())  eeFile->generalDefault();
    eeFile->getGeneralSettings(&g_eeGeneral);
    eeFile->getModel(&g_model,id);
    id_model = id;

    QSettings settings("er9x-eePe", "eePe");
    ui->tabWidget->setCurrentIndex(settings.value("modelEditTab", 0).toInt());

    tabModelEditSetup();
    tabExpo();
    tabMixes();
    tabLimits();
    tabCurves();
    tabSwitches();
    tabTrims();

    ui->curvePreview->setMinimumWidth(260);
    ui->curvePreview->setMinimumHeight(260);

}

ModelEdit::~ModelEdit()
{
    delete ui;
}

void ModelEdit::resizeEvent(QResizeEvent *event)
{
    if(event->oldSize().isValid())
        ui->curvePreview->scale((qreal)event->size().width()/event->oldSize().width(),(qreal)event->size().height()/event->oldSize().height());
}

void ModelEdit::updateSettings()
{
    eeFile->putModel(&g_model,id_model);
    emit modelValuesChanged();
}

void ModelEdit::on_tabWidget_currentChanged(int index)
{
    QSettings settings("er9x-eePe", "eePe");
    settings.setValue("modelEditTab",index);//ui->tabWidget->currentIndex());
}


void ModelEdit::tabModelEditSetup()
{
    //name
    ui->modelNameLE->setText(g_model.name);

    //timer mode direction value
    populateTimerSwitchCB(ui->timerModeCB,g_model.tmrMode);
    int min = g_model.tmrVal/60;
    int sec = g_model.tmrVal%60;
    ui->timerValTE->setTime(QTime(0,min,sec));

    //trim inc, thro trim, thro expo, instatrim
    ui->trimIncCB->setCurrentIndex(g_model.trimInc);
    populateSwitchCB(ui->trimSWCB,g_model.trimSw);
    ui->thrExpoChkB->setChecked(g_model.thrExpo);
    ui->thrTrimChkB->setChecked(g_model.thrTrim);

    //center beep
    ui->bcRUDChkB->setChecked(g_model.beepANACenter & BC_BIT_RUD);
    ui->bcELEChkB->setChecked(g_model.beepANACenter & BC_BIT_ELE);
    ui->bcTHRChkB->setChecked(g_model.beepANACenter & BC_BIT_THR);
    ui->bcAILChkB->setChecked(g_model.beepANACenter & BC_BIT_AIL);
    ui->bcP1ChkB->setChecked(g_model.beepANACenter & BC_BIT_P1);
    ui->bcP2ChkB->setChecked(g_model.beepANACenter & BC_BIT_P2);
    ui->bcP3ChkB->setChecked(g_model.beepANACenter & BC_BIT_P3);

    //pulse polarity
    ui->pulsePolCB->setCurrentIndex(g_model.pulsePol);

    //protocol channels ppm delay (disable if needed)
    ui->protocolCB->setCurrentIndex(g_model.protocol);
    ui->ppmDelaySB->setValue(300+50*g_model.ppmDelay);
    ui->numChannelsSB->setValue(8+2*g_model.ppmNCH);
    ui->ppmDelaySB->setEnabled(!g_model.protocol);
    ui->numChannelsSB->setEnabled(!g_model.protocol);
}

void ModelEdit::tabExpo()
{
    populateSwitchCB(ui->RUD_edrSw1,g_model.expoData[CONVERT_MODE(RUD)-1].drSw1);
    populateSwitchCB(ui->RUD_edrSw2,g_model.expoData[CONVERT_MODE(RUD)-1].drSw2);
    populateSwitchCB(ui->ELE_edrSw1,g_model.expoData[CONVERT_MODE(ELE)-1].drSw1);
    populateSwitchCB(ui->ELE_edrSw2,g_model.expoData[CONVERT_MODE(ELE)-1].drSw2);
    populateSwitchCB(ui->THR_edrSw1,g_model.expoData[CONVERT_MODE(THR)-1].drSw1);
    populateSwitchCB(ui->THR_edrSw2,g_model.expoData[CONVERT_MODE(THR)-1].drSw2);
    populateSwitchCB(ui->AIL_edrSw1,g_model.expoData[CONVERT_MODE(AIL)-1].drSw1);
    populateSwitchCB(ui->AIL_edrSw2,g_model.expoData[CONVERT_MODE(AIL)-1].drSw2);



//#define DR_HIGH   0
//#define DR_MID    1
//#define DR_LOW    2
//#define DR_EXPO   0
//#define DR_WEIGHT 1
//#define DR_RIGHT  0
//#define DR_LEFT   1
//expo[3][2][2] //[HI/MID/LOW][expo/weight][R/L]
    ui->RUD_DrLHi->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_HIGH][DR_WEIGHT][DR_LEFT]+100);
    ui->RUD_DrLLow->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_LOW][DR_WEIGHT][DR_LEFT]+100);
    ui->RUD_DrLMid->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_MID][DR_WEIGHT][DR_LEFT]+100);
    ui->RUD_DrRHi->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_HIGH][DR_WEIGHT][DR_RIGHT]+100);
    ui->RUD_DrRLow->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_LOW][DR_WEIGHT][DR_RIGHT]+100);
    ui->RUD_DrRMid->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_MID][DR_WEIGHT][DR_RIGHT]+100);
    ui->RUD_ExpoLHi->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_HIGH][DR_EXPO][DR_LEFT]);
    ui->RUD_ExpoLLow->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_LOW][DR_EXPO][DR_LEFT]);
    ui->RUD_ExpoLMid->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_MID][DR_EXPO][DR_LEFT]);
    ui->RUD_ExpoRHi->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_HIGH][DR_EXPO][DR_RIGHT]);
    ui->RUD_ExpoRLow->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_LOW][DR_EXPO][DR_RIGHT]);
    ui->RUD_ExpoRMid->setValue(g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_MID][DR_EXPO][DR_RIGHT]);

    ui->ELE_DrLHi->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_HIGH][DR_WEIGHT][DR_LEFT]+100);
    ui->ELE_DrLLow->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_LOW][DR_WEIGHT][DR_LEFT]+100);
    ui->ELE_DrLMid->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_MID][DR_WEIGHT][DR_LEFT]+100);
    ui->ELE_DrRHi->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_HIGH][DR_WEIGHT][DR_RIGHT]+100);
    ui->ELE_DrRLow->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_LOW][DR_WEIGHT][DR_RIGHT]+100);
    ui->ELE_DrRMid->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_MID][DR_WEIGHT][DR_RIGHT]+100);
    ui->ELE_ExpoLHi->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_HIGH][DR_EXPO][DR_LEFT]);
    ui->ELE_ExpoLLow->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_LOW][DR_EXPO][DR_LEFT]);
    ui->ELE_ExpoLMid->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_MID][DR_EXPO][DR_LEFT]);
    ui->ELE_ExpoRHi->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_HIGH][DR_EXPO][DR_RIGHT]);
    ui->ELE_ExpoRLow->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_LOW][DR_EXPO][DR_RIGHT]);
    ui->ELE_ExpoRMid->setValue(g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_MID][DR_EXPO][DR_RIGHT]);

    ui->THR_DrLHi->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_HIGH][DR_WEIGHT][DR_LEFT]+100);
    ui->THR_DrLLow->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_LOW][DR_WEIGHT][DR_LEFT]+100);
    ui->THR_DrLMid->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_MID][DR_WEIGHT][DR_LEFT]+100);
    ui->THR_DrRHi->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_HIGH][DR_WEIGHT][DR_RIGHT]+100);
    ui->THR_DrRLow->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_LOW][DR_WEIGHT][DR_RIGHT]+100);
    ui->THR_DrRMid->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_MID][DR_WEIGHT][DR_RIGHT]+100);
    ui->THR_ExpoLHi->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_HIGH][DR_EXPO][DR_LEFT]);
    ui->THR_ExpoLLow->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_LOW][DR_EXPO][DR_LEFT]);
    ui->THR_ExpoLMid->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_MID][DR_EXPO][DR_LEFT]);
    ui->THR_ExpoRHi->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_HIGH][DR_EXPO][DR_RIGHT]);
    ui->THR_ExpoRLow->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_LOW][DR_EXPO][DR_RIGHT]);
    ui->THR_ExpoRMid->setValue(g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_MID][DR_EXPO][DR_RIGHT]);

    ui->AIL_DrLHi->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_HIGH][DR_WEIGHT][DR_LEFT]+100);
    ui->AIL_DrLLow->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_LOW][DR_WEIGHT][DR_LEFT]+100);
    ui->AIL_DrLMid->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_MID][DR_WEIGHT][DR_LEFT]+100);
    ui->AIL_DrRHi->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_HIGH][DR_WEIGHT][DR_RIGHT]+100);
    ui->AIL_DrRLow->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_LOW][DR_WEIGHT][DR_RIGHT]+100);
    ui->AIL_DrRMid->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_MID][DR_WEIGHT][DR_RIGHT]+100);
    ui->AIL_ExpoLHi->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_HIGH][DR_EXPO][DR_LEFT]);
    ui->AIL_ExpoLLow->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_LOW][DR_EXPO][DR_LEFT]);
    ui->AIL_ExpoLMid->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_MID][DR_EXPO][DR_LEFT]);
    ui->AIL_ExpoRHi->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_HIGH][DR_EXPO][DR_RIGHT]);
    ui->AIL_ExpoRLow->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_LOW][DR_EXPO][DR_RIGHT]);
    ui->AIL_ExpoRMid->setValue(g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_MID][DR_EXPO][DR_RIGHT]);

    connect(ui->RUD_edrSw1,SIGNAL(currentIndexChanged(int)),this,SLOT(expoEdited()));
    connect(ui->RUD_edrSw2,SIGNAL(currentIndexChanged(int)),this,SLOT(expoEdited()));
    connect(ui->ELE_edrSw1,SIGNAL(currentIndexChanged(int)),this,SLOT(expoEdited()));
    connect(ui->ELE_edrSw2,SIGNAL(currentIndexChanged(int)),this,SLOT(expoEdited()));
    connect(ui->THR_edrSw1,SIGNAL(currentIndexChanged(int)),this,SLOT(expoEdited()));
    connect(ui->THR_edrSw2,SIGNAL(currentIndexChanged(int)),this,SLOT(expoEdited()));
    connect(ui->AIL_edrSw1,SIGNAL(currentIndexChanged(int)),this,SLOT(expoEdited()));
    connect(ui->AIL_edrSw2,SIGNAL(currentIndexChanged(int)),this,SLOT(expoEdited()));

    connect(ui->RUD_DrLHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_DrLLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_DrLMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_DrRHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_DrRLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_DrRMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_ExpoLHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_ExpoLLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_ExpoLMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_ExpoRHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_ExpoRLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->RUD_ExpoRMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));

    connect(ui->ELE_DrLHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_DrLLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_DrLMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_DrRHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_DrRLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_DrRMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_ExpoLHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_ExpoLLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_ExpoLMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_ExpoRHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_ExpoRLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->ELE_ExpoRMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));

    connect(ui->THR_DrLHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_DrLLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_DrLMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_DrRHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_DrRLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_DrRMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_ExpoLHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_ExpoLLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_ExpoLMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_ExpoRHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_ExpoRLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->THR_ExpoRMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));

    connect(ui->AIL_DrLHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_DrLLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_DrLMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_DrRHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_DrRLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_DrRMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_ExpoLHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_ExpoLLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_ExpoLMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_ExpoRHi,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_ExpoRLow,SIGNAL(editingFinished()),this,SLOT(expoEdited()));
    connect(ui->AIL_ExpoRMid,SIGNAL(editingFinished()),this,SLOT(expoEdited()));

}


void ModelEdit::expoEdited()
{
    g_model.expoData[CONVERT_MODE(RUD)-1].drSw1 = ui->RUD_edrSw1->currentIndex()-MAX_DRSWITCH;
    g_model.expoData[CONVERT_MODE(RUD)-1].drSw2 = ui->RUD_edrSw2->currentIndex()-MAX_DRSWITCH;
    g_model.expoData[CONVERT_MODE(ELE)-1].drSw1 = ui->ELE_edrSw1->currentIndex()-MAX_DRSWITCH;
    g_model.expoData[CONVERT_MODE(ELE)-1].drSw2 = ui->ELE_edrSw2->currentIndex()-MAX_DRSWITCH;
    g_model.expoData[CONVERT_MODE(THR)-1].drSw1 = ui->THR_edrSw1->currentIndex()-MAX_DRSWITCH;
    g_model.expoData[CONVERT_MODE(THR)-1].drSw2 = ui->THR_edrSw2->currentIndex()-MAX_DRSWITCH;
    g_model.expoData[CONVERT_MODE(AIL)-1].drSw1 = ui->AIL_edrSw1->currentIndex()-MAX_DRSWITCH;
    g_model.expoData[CONVERT_MODE(AIL)-1].drSw2 = ui->AIL_edrSw2->currentIndex()-MAX_DRSWITCH;

    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_HIGH][DR_WEIGHT][DR_LEFT]  = ui->RUD_DrLHi->value()-100;
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_LOW][DR_WEIGHT][DR_LEFT]   = ui->RUD_DrLLow->value()-100;
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_MID][DR_WEIGHT][DR_LEFT]   = ui->RUD_DrLMid->value()-100;
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_HIGH][DR_WEIGHT][DR_RIGHT] = ui->RUD_DrRHi->value()-100;
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_LOW][DR_WEIGHT][DR_RIGHT]  = ui->RUD_DrRLow->value()-100;
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_MID][DR_WEIGHT][DR_RIGHT]  = ui->RUD_DrRMid->value()-100;
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_HIGH][DR_EXPO][DR_LEFT]    = ui->RUD_ExpoLHi->value();
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_LOW][DR_EXPO][DR_LEFT]     = ui->RUD_ExpoLLow->value();
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_MID][DR_EXPO][DR_LEFT]     = ui->RUD_ExpoLMid->value();
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_HIGH][DR_EXPO][DR_RIGHT]   = ui->RUD_ExpoRHi->value();
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_LOW][DR_EXPO][DR_RIGHT]    = ui->RUD_ExpoRLow->value();
    g_model.expoData[CONVERT_MODE(RUD)-1].expo[DR_MID][DR_EXPO][DR_RIGHT]    = ui->RUD_ExpoRMid->value();

    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_HIGH][DR_WEIGHT][DR_LEFT]  = ui->ELE_DrLHi->value()-100;
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_LOW][DR_WEIGHT][DR_LEFT]   = ui->ELE_DrLLow->value()-100;
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_MID][DR_WEIGHT][DR_LEFT]   = ui->ELE_DrLMid->value()-100;
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_HIGH][DR_WEIGHT][DR_RIGHT] = ui->ELE_DrRHi->value()-100;
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_LOW][DR_WEIGHT][DR_RIGHT]  = ui->ELE_DrRLow->value()-100;
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_MID][DR_WEIGHT][DR_RIGHT]  = ui->ELE_DrRMid->value()-100;
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_HIGH][DR_EXPO][DR_LEFT]    = ui->ELE_ExpoLHi->value();
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_LOW][DR_EXPO][DR_LEFT]     = ui->ELE_ExpoLLow->value();
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_MID][DR_EXPO][DR_LEFT]     = ui->ELE_ExpoLMid->value();
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_HIGH][DR_EXPO][DR_RIGHT]   = ui->ELE_ExpoRHi->value();
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_LOW][DR_EXPO][DR_RIGHT]    = ui->ELE_ExpoRLow->value();
    g_model.expoData[CONVERT_MODE(ELE)-1].expo[DR_MID][DR_EXPO][DR_RIGHT]    = ui->ELE_ExpoRMid->value();

    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_HIGH][DR_WEIGHT][DR_LEFT]  = ui->THR_DrLHi->value()-100;
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_LOW][DR_WEIGHT][DR_LEFT]   = ui->THR_DrLLow->value()-100;
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_MID][DR_WEIGHT][DR_LEFT]   = ui->THR_DrLMid->value()-100;
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_HIGH][DR_WEIGHT][DR_RIGHT] = ui->THR_DrRHi->value()-100;
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_LOW][DR_WEIGHT][DR_RIGHT]  = ui->THR_DrRLow->value()-100;
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_MID][DR_WEIGHT][DR_RIGHT]  = ui->THR_DrRMid->value()-100;
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_HIGH][DR_EXPO][DR_LEFT]    = ui->THR_ExpoLHi->value();
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_LOW][DR_EXPO][DR_LEFT]     = ui->THR_ExpoLLow->value();
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_MID][DR_EXPO][DR_LEFT]     = ui->THR_ExpoLMid->value();
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_HIGH][DR_EXPO][DR_RIGHT]   = ui->THR_ExpoRHi->value();
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_LOW][DR_EXPO][DR_RIGHT]    = ui->THR_ExpoRLow->value();
    g_model.expoData[CONVERT_MODE(THR)-1].expo[DR_MID][DR_EXPO][DR_RIGHT]    = ui->THR_ExpoRMid->value();

    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_HIGH][DR_WEIGHT][DR_LEFT]  = ui->AIL_DrLHi->value()-100;
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_LOW][DR_WEIGHT][DR_LEFT]   = ui->AIL_DrLLow->value()-100;
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_MID][DR_WEIGHT][DR_LEFT]   = ui->AIL_DrLMid->value()-100;
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_HIGH][DR_WEIGHT][DR_RIGHT] = ui->AIL_DrRHi->value()-100;
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_LOW][DR_WEIGHT][DR_RIGHT]  = ui->AIL_DrRLow->value()-100;
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_MID][DR_WEIGHT][DR_RIGHT]  = ui->AIL_DrRMid->value()-100;
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_HIGH][DR_EXPO][DR_LEFT]    = ui->AIL_ExpoLHi->value();
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_LOW][DR_EXPO][DR_LEFT]     = ui->AIL_ExpoLLow->value();
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_MID][DR_EXPO][DR_LEFT]     = ui->AIL_ExpoLMid->value();
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_HIGH][DR_EXPO][DR_RIGHT]   = ui->AIL_ExpoRHi->value();
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_LOW][DR_EXPO][DR_RIGHT]    = ui->AIL_ExpoRLow->value();
    g_model.expoData[CONVERT_MODE(AIL)-1].expo[DR_MID][DR_EXPO][DR_RIGHT]    = ui->AIL_ExpoRMid->value();

    updateSettings();
}


void ModelEdit::tabMixes()
{
    int curDest = 0;
    for(int i=0; i<MAX_MIXERS; i++)
    {
        MixData *md = &g_model.mixData[i];
        if(!md->destCh) break;
        QString str = "";
        while(curDest<(md->destCh-1))
        {
            curDest++;
            str = tr("CH%1%2").arg(curDest/10).arg(curDest%10);
            ui->MixerlistWidget->addItem(str);
        }

        if(curDest!=md->destCh)
        {
            str = tr("CH%1%2").arg(md->destCh/10).arg(md->destCh%10);
            curDest=md->destCh;
        }
        else
            str = "    ";

        switch(md->mltpx)
        {
        case (1): str += " *"; break;
        case (2): str += " R"; break;
        default:  str += "  "; break;
        };

        str += md->weight<0 ? tr(" %1\%").arg(md->weight).rightJustified(6,' ') :
                              tr(" +%1\%").arg(md->weight).rightJustified(6, ' ');


        QString srcStr = SRC_STR;
        str += " " + srcStr.mid(CONVERT_MODE(md->srcRaw+1)*4,4);

        if(md->swtch)
        {
            QString swtStr = SWITCHES_STR;
            str += tr("Switch(%1%2)").arg(md->swtch<0 ? "!" : "").arg(swtStr.mid((abs(md->swtch)-1)*3,3));
        }

        if(md->carryTrim) str += " noTrim";
        if(md->sOffset)  str += tr(" Offset(%1\%)").arg(md->sOffset);
        if(md->curve)
        {
            QString crvStr = CURV_STR;
            str += tr(" Curve(%1)").arg(crvStr.mid(md->curve*3,3).remove(' '));
        }

        if(md->delayDown || md->delayUp) str += tr(" Delay(u%1:d%2)").arg(md->delayUp).arg(md->delayDown);
        if(md->speedDown || md->speedUp) str += tr(" Slow(u%1:d%2)").arg(md->speedUp).arg(md->speedDown);

        if(md->mixWarn)  str += tr(" Warn(%1)").arg(md->mixWarn);

        ui->MixerlistWidget->addItem(str);
    }

    while(curDest<NUM_XCHNOUT)
    {
        curDest++;
        QString str = tr("CH%1%2").arg(curDest/10).arg(curDest%10);
        ui->MixerlistWidget->addItem(str);
    }

}

void ModelEdit::mixesEdited()
{
    updateSettings();
}

void ModelEdit::tabLimits()
{
    ui->offsetDSB_1->setValue(g_model.limitData[0].offset/10);   connect(ui->offsetDSB_1,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_2->setValue(g_model.limitData[1].offset/10);   connect(ui->offsetDSB_2,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_3->setValue(g_model.limitData[2].offset/10);   connect(ui->offsetDSB_3,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_4->setValue(g_model.limitData[3].offset/10);   connect(ui->offsetDSB_4,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_5->setValue(g_model.limitData[4].offset/10);   connect(ui->offsetDSB_5,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_6->setValue(g_model.limitData[5].offset/10);   connect(ui->offsetDSB_6,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_7->setValue(g_model.limitData[6].offset/10);   connect(ui->offsetDSB_7,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_8->setValue(g_model.limitData[7].offset/10);   connect(ui->offsetDSB_8,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_9->setValue(g_model.limitData[8].offset/10);   connect(ui->offsetDSB_9,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_10->setValue(g_model.limitData[9].offset/10);  connect(ui->offsetDSB_10,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_11->setValue(g_model.limitData[10].offset/10); connect(ui->offsetDSB_11,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_12->setValue(g_model.limitData[11].offset/10); connect(ui->offsetDSB_12,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_13->setValue(g_model.limitData[12].offset/10); connect(ui->offsetDSB_13,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_14->setValue(g_model.limitData[13].offset/10); connect(ui->offsetDSB_14,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_15->setValue(g_model.limitData[14].offset/10); connect(ui->offsetDSB_15,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_16->setValue(g_model.limitData[15].offset/10); connect(ui->offsetDSB_16,SIGNAL(editingFinished()),this,SLOT(limitEdited()));

    ui->minSB_1->setValue(g_model.limitData[0].min-100);   connect(ui->minSB_1,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_2->setValue(g_model.limitData[1].min-100);   connect(ui->minSB_2,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_3->setValue(g_model.limitData[2].min-100);   connect(ui->minSB_3,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_4->setValue(g_model.limitData[3].min-100);   connect(ui->minSB_4,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_5->setValue(g_model.limitData[4].min-100);   connect(ui->minSB_5,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_6->setValue(g_model.limitData[5].min-100);   connect(ui->minSB_6,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_7->setValue(g_model.limitData[6].min-100);   connect(ui->minSB_7,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_8->setValue(g_model.limitData[7].min-100);   connect(ui->minSB_8,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_9->setValue(g_model.limitData[8].min-100);   connect(ui->minSB_9,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_10->setValue(g_model.limitData[9].min-100);  connect(ui->minSB_10,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_11->setValue(g_model.limitData[10].min-100); connect(ui->minSB_11,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_12->setValue(g_model.limitData[11].min-100); connect(ui->minSB_12,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_13->setValue(g_model.limitData[12].min-100); connect(ui->minSB_13,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_14->setValue(g_model.limitData[13].min-100); connect(ui->minSB_14,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_15->setValue(g_model.limitData[14].min-100); connect(ui->minSB_15,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_16->setValue(g_model.limitData[15].min-100); connect(ui->minSB_16,SIGNAL(editingFinished()),this,SLOT(limitEdited()));

    ui->maxSB_1->setValue(g_model.limitData[0].max+100);   connect(ui->maxSB_1,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_2->setValue(g_model.limitData[1].max+100);   connect(ui->maxSB_2,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_3->setValue(g_model.limitData[2].max+100);   connect(ui->maxSB_3,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_4->setValue(g_model.limitData[3].max+100);   connect(ui->maxSB_4,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_5->setValue(g_model.limitData[4].max+100);   connect(ui->maxSB_5,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_6->setValue(g_model.limitData[5].max+100);   connect(ui->maxSB_6,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_7->setValue(g_model.limitData[6].max+100);   connect(ui->maxSB_7,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_8->setValue(g_model.limitData[7].max+100);   connect(ui->maxSB_8,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_9->setValue(g_model.limitData[8].max+100);   connect(ui->maxSB_9,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_10->setValue(g_model.limitData[9].max+100);  connect(ui->maxSB_10,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_11->setValue(g_model.limitData[10].max+100); connect(ui->maxSB_11,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_12->setValue(g_model.limitData[11].max+100); connect(ui->maxSB_12,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_13->setValue(g_model.limitData[12].max+100); connect(ui->maxSB_13,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_14->setValue(g_model.limitData[13].max+100); connect(ui->maxSB_14,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_15->setValue(g_model.limitData[14].max+100); connect(ui->maxSB_15,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_16->setValue(g_model.limitData[15].max+100); connect(ui->maxSB_16,SIGNAL(editingFinished()),this,SLOT(limitEdited()));

    ui->chInvCB_1->setCurrentIndex((g_model.limitData[0].revert) ? 1 : 0);   connect(ui->chInvCB_1,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_2->setCurrentIndex((g_model.limitData[1].revert) ? 1 : 0);   connect(ui->chInvCB_2,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_3->setCurrentIndex((g_model.limitData[2].revert) ? 1 : 0);   connect(ui->chInvCB_3,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_4->setCurrentIndex((g_model.limitData[3].revert) ? 1 : 0);   connect(ui->chInvCB_4,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_5->setCurrentIndex((g_model.limitData[4].revert) ? 1 : 0);   connect(ui->chInvCB_5,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_6->setCurrentIndex((g_model.limitData[5].revert) ? 1 : 0);   connect(ui->chInvCB_6,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_7->setCurrentIndex((g_model.limitData[6].revert) ? 1 : 0);   connect(ui->chInvCB_7,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_8->setCurrentIndex((g_model.limitData[7].revert) ? 1 : 0);   connect(ui->chInvCB_8,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_9->setCurrentIndex((g_model.limitData[8].revert) ? 1 : 0);   connect(ui->chInvCB_9,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_10->setCurrentIndex((g_model.limitData[9].revert) ? 1 : 0);  connect(ui->chInvCB_10,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_11->setCurrentIndex((g_model.limitData[10].revert) ? 1 : 0); connect(ui->chInvCB_11,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_12->setCurrentIndex((g_model.limitData[11].revert) ? 1 : 0); connect(ui->chInvCB_12,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_13->setCurrentIndex((g_model.limitData[12].revert) ? 1 : 0); connect(ui->chInvCB_13,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_14->setCurrentIndex((g_model.limitData[13].revert) ? 1 : 0); connect(ui->chInvCB_14,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_15->setCurrentIndex((g_model.limitData[14].revert) ? 1 : 0); connect(ui->chInvCB_15,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_16->setCurrentIndex((g_model.limitData[15].revert) ? 1 : 0); connect(ui->chInvCB_16,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
}

void ModelEdit::updateTabCurves()
{
   ui->curvePt1_1->setValue(g_model.curves5[0][0]);    
   ui->curvePt2_1->setValue(g_model.curves5[0][1]);
   ui->curvePt3_1->setValue(g_model.curves5[0][2]);
   ui->curvePt4_1->setValue(g_model.curves5[0][3]);
   ui->curvePt5_1->setValue(g_model.curves5[0][4]);
   
   ui->curvePt1_2->setValue(g_model.curves5[1][0]);
   ui->curvePt2_2->setValue(g_model.curves5[1][1]);
   ui->curvePt3_2->setValue(g_model.curves5[1][2]);
   ui->curvePt4_2->setValue(g_model.curves5[1][3]);
   ui->curvePt5_2->setValue(g_model.curves5[1][4]);
   
   ui->curvePt1_3->setValue(g_model.curves5[2][0]);
   ui->curvePt2_3->setValue(g_model.curves5[2][1]);
   ui->curvePt3_3->setValue(g_model.curves5[2][2]);
   ui->curvePt4_3->setValue(g_model.curves5[2][3]);
   ui->curvePt5_3->setValue(g_model.curves5[2][4]);
   
   ui->curvePt1_4->setValue(g_model.curves5[3][0]);
   ui->curvePt2_4->setValue(g_model.curves5[3][1]);
   ui->curvePt3_4->setValue(g_model.curves5[3][2]);
   ui->curvePt4_4->setValue(g_model.curves5[3][3]);
   ui->curvePt5_4->setValue(g_model.curves5[3][4]);
	
   ui->curvePt1_5->setValue(g_model.curves5[4][0]);
   ui->curvePt2_5->setValue(g_model.curves5[4][1]);
   ui->curvePt3_5->setValue(g_model.curves5[4][2]);
   ui->curvePt4_5->setValue(g_model.curves5[4][3]);
   ui->curvePt5_5->setValue(g_model.curves5[4][4]);
	
   ui->curvePt1_6->setValue(g_model.curves5[5][0]);
   ui->curvePt2_6->setValue(g_model.curves5[5][1]);
   ui->curvePt3_6->setValue(g_model.curves5[5][2]);
   ui->curvePt4_6->setValue(g_model.curves5[5][3]);
   ui->curvePt5_6->setValue(g_model.curves5[5][4]);
	
   ui->curvePt1_7->setValue(g_model.curves5[6][0]);
   ui->curvePt2_7->setValue(g_model.curves5[6][1]);
   ui->curvePt3_7->setValue(g_model.curves5[6][2]);
   ui->curvePt4_7->setValue(g_model.curves5[6][3]);
   ui->curvePt5_7->setValue(g_model.curves5[6][4]);
	
   ui->curvePt1_8->setValue(g_model.curves5[7][0]);
   ui->curvePt2_8->setValue(g_model.curves5[7][1]);
   ui->curvePt3_8->setValue(g_model.curves5[7][2]);
   ui->curvePt4_8->setValue(g_model.curves5[7][3]);
   ui->curvePt5_8->setValue(g_model.curves5[7][4]);
   
   ui->curvePt1_9->setValue(g_model.curves9[0][0]);
   ui->curvePt2_9->setValue(g_model.curves9[0][1]);
   ui->curvePt3_9->setValue(g_model.curves9[0][2]);
   ui->curvePt4_9->setValue(g_model.curves9[0][3]);
   ui->curvePt5_9->setValue(g_model.curves9[0][4]);
   ui->curvePt6_9->setValue(g_model.curves9[0][5]);
   ui->curvePt7_9->setValue(g_model.curves9[0][6]);
   ui->curvePt8_9->setValue(g_model.curves9[0][7]);
   ui->curvePt9_9->setValue(g_model.curves9[0][8]);
   
   ui->curvePt1_10->setValue(g_model.curves9[1][0]);
   ui->curvePt2_10->setValue(g_model.curves9[1][1]);
   ui->curvePt3_10->setValue(g_model.curves9[1][2]);
   ui->curvePt4_10->setValue(g_model.curves9[1][3]);
   ui->curvePt5_10->setValue(g_model.curves9[1][4]);
   ui->curvePt6_10->setValue(g_model.curves9[1][5]);
   ui->curvePt7_10->setValue(g_model.curves9[1][6]);
   ui->curvePt8_10->setValue(g_model.curves9[1][7]);
   ui->curvePt9_10->setValue(g_model.curves9[1][8]);
   
   ui->curvePt1_11->setValue(g_model.curves9[2][0]);
   ui->curvePt2_11->setValue(g_model.curves9[2][1]);
   ui->curvePt3_11->setValue(g_model.curves9[2][2]);
   ui->curvePt4_11->setValue(g_model.curves9[2][3]);
   ui->curvePt5_11->setValue(g_model.curves9[2][4]);
   ui->curvePt6_11->setValue(g_model.curves9[2][5]);
   ui->curvePt7_11->setValue(g_model.curves9[2][6]);
   ui->curvePt8_11->setValue(g_model.curves9[2][7]);
   ui->curvePt9_11->setValue(g_model.curves9[2][8]);
   
   ui->curvePt1_12->setValue(g_model.curves9[3][0]);
   ui->curvePt2_12->setValue(g_model.curves9[3][1]);
   ui->curvePt3_12->setValue(g_model.curves9[3][2]);
   ui->curvePt4_12->setValue(g_model.curves9[3][3]);
   ui->curvePt5_12->setValue(g_model.curves9[3][4]);
   ui->curvePt6_12->setValue(g_model.curves9[3][5]);
   ui->curvePt7_12->setValue(g_model.curves9[3][6]);
   ui->curvePt8_12->setValue(g_model.curves9[3][7]);
   ui->curvePt9_12->setValue(g_model.curves9[3][8]);
   
   ui->curvePt1_13->setValue(g_model.curves9[4][0]);
   ui->curvePt2_13->setValue(g_model.curves9[4][1]);
   ui->curvePt3_13->setValue(g_model.curves9[4][2]);
   ui->curvePt4_13->setValue(g_model.curves9[4][3]);
   ui->curvePt5_13->setValue(g_model.curves9[4][4]);
   ui->curvePt6_13->setValue(g_model.curves9[4][5]);
   ui->curvePt7_13->setValue(g_model.curves9[4][6]);
   ui->curvePt8_13->setValue(g_model.curves9[4][7]);
   ui->curvePt9_13->setValue(g_model.curves9[4][8]);
   
   ui->curvePt1_14->setValue(g_model.curves9[5][0]);
   ui->curvePt2_14->setValue(g_model.curves9[5][1]);
   ui->curvePt3_14->setValue(g_model.curves9[5][2]);
   ui->curvePt4_14->setValue(g_model.curves9[5][3]);
   ui->curvePt5_14->setValue(g_model.curves9[5][4]);
   ui->curvePt6_14->setValue(g_model.curves9[5][5]);
   ui->curvePt7_14->setValue(g_model.curves9[5][6]);
   ui->curvePt8_14->setValue(g_model.curves9[5][7]);
   ui->curvePt9_14->setValue(g_model.curves9[5][8]);
   
   ui->curvePt1_15->setValue(g_model.curves9[6][0]);
   ui->curvePt2_15->setValue(g_model.curves9[6][1]);
   ui->curvePt3_15->setValue(g_model.curves9[6][2]);
   ui->curvePt4_15->setValue(g_model.curves9[6][3]);
   ui->curvePt5_15->setValue(g_model.curves9[6][4]);
   ui->curvePt6_15->setValue(g_model.curves9[6][5]);
   ui->curvePt7_15->setValue(g_model.curves9[6][6]);
   ui->curvePt8_15->setValue(g_model.curves9[6][7]);
   ui->curvePt9_15->setValue(g_model.curves9[6][8]);
   
   ui->curvePt1_16->setValue(g_model.curves9[7][0]);
   ui->curvePt2_16->setValue(g_model.curves9[7][1]);
   ui->curvePt3_16->setValue(g_model.curves9[7][2]);
   ui->curvePt4_16->setValue(g_model.curves9[7][3]);
   ui->curvePt5_16->setValue(g_model.curves9[7][4]);
   ui->curvePt6_16->setValue(g_model.curves9[7][5]);
   ui->curvePt7_16->setValue(g_model.curves9[7][6]);
   ui->curvePt8_16->setValue(g_model.curves9[7][7]);
   ui->curvePt9_16->setValue(g_model.curves9[7][8]);
}


void ModelEdit::tabCurves()
{
   updateTabCurves();

   QGraphicsScene *scene = new QGraphicsScene(ui->curvePreview);
   scene->setItemIndexMethod(QGraphicsScene::NoIndex);
   QSize gs = ui->curvePreview->size();
   scene->setSceneRect(-gs.width()/2, -gs.height()/2, gs.width(), gs.height());
   ui->curvePreview->setScene(scene);
   curveEdit_clicked(1);



   connect(ui->curvePt1_1,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_1,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_1,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_1,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_1,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_2,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_2,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_2,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_2,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_2,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_3,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_3,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_3,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_3,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_3,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_4,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_4,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_4,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_4,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_4,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_5,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_5,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_5,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_5,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_5,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_6,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_6,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_6,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_6,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_6,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_7,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_7,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_7,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_7,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_7,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_8,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_8,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_8,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_8,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_8,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_9,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_10,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_11,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_12,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_13,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_14,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_15,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_16,SIGNAL(editingFinished()),this,SLOT(curvePointEdited()));
}


void ModelEdit::limitEdited()
{
    g_model.limitData[0].offset = ui->offsetDSB_1->value()*10;
    g_model.limitData[1].offset = ui->offsetDSB_2->value()*10;
    g_model.limitData[2].offset = ui->offsetDSB_3->value()*10;
    g_model.limitData[3].offset = ui->offsetDSB_4->value()*10;
    g_model.limitData[4].offset = ui->offsetDSB_5->value()*10;
    g_model.limitData[5].offset = ui->offsetDSB_6->value()*10;
    g_model.limitData[6].offset = ui->offsetDSB_7->value()*10;
    g_model.limitData[7].offset = ui->offsetDSB_8->value()*10;
    g_model.limitData[8].offset = ui->offsetDSB_9->value()*10;
    g_model.limitData[9].offset = ui->offsetDSB_10->value()*10;
    g_model.limitData[10].offset = ui->offsetDSB_11->value()*10;
    g_model.limitData[11].offset = ui->offsetDSB_12->value()*10;
    g_model.limitData[12].offset = ui->offsetDSB_13->value()*10;
    g_model.limitData[13].offset = ui->offsetDSB_14->value()*10;
    g_model.limitData[14].offset = ui->offsetDSB_15->value()*10;
    g_model.limitData[15].offset = ui->offsetDSB_16->value()*10;

    g_model.limitData[0].min = ui->minSB_1->value()+100;
    g_model.limitData[1].min = ui->minSB_2->value()+100;
    g_model.limitData[2].min = ui->minSB_3->value()+100;
    g_model.limitData[3].min = ui->minSB_4->value()+100;
    g_model.limitData[4].min = ui->minSB_5->value()+100;
    g_model.limitData[5].min = ui->minSB_6->value()+100;
    g_model.limitData[6].min = ui->minSB_7->value()+100;
    g_model.limitData[7].min = ui->minSB_8->value()+100;
    g_model.limitData[8].min = ui->minSB_9->value()+100;
    g_model.limitData[9].min = ui->minSB_10->value()+100;
    g_model.limitData[10].min = ui->minSB_11->value()+100;
    g_model.limitData[11].min = ui->minSB_12->value()+100;
    g_model.limitData[12].min = ui->minSB_13->value()+100;
    g_model.limitData[13].min = ui->minSB_14->value()+100;
    g_model.limitData[14].min = ui->minSB_15->value()+100;
    g_model.limitData[15].min = ui->minSB_16->value()+100;

    g_model.limitData[0].max = ui->maxSB_1->value()-100;
    g_model.limitData[1].max = ui->maxSB_2->value()-100;
    g_model.limitData[2].max = ui->maxSB_3->value()-100;
    g_model.limitData[3].max = ui->maxSB_4->value()-100;
    g_model.limitData[4].max = ui->maxSB_5->value()-100;
    g_model.limitData[5].max = ui->maxSB_6->value()-100;
    g_model.limitData[6].max = ui->maxSB_7->value()-100;
    g_model.limitData[7].max = ui->maxSB_8->value()-100;
    g_model.limitData[8].max = ui->maxSB_9->value()-100;
    g_model.limitData[9].max = ui->maxSB_10->value()-100;
    g_model.limitData[10].max = ui->maxSB_11->value()-100;
    g_model.limitData[11].max = ui->maxSB_12->value()-100;
    g_model.limitData[12].max = ui->maxSB_13->value()-100;
    g_model.limitData[13].max = ui->maxSB_14->value()-100;
    g_model.limitData[14].max = ui->maxSB_15->value()-100;
    g_model.limitData[15].max = ui->maxSB_16->value()-100;

    g_model.limitData[0].revert = ui->chInvCB_1->currentIndex();
    g_model.limitData[1].revert = ui->chInvCB_2->currentIndex();
    g_model.limitData[2].revert = ui->chInvCB_3->currentIndex();
    g_model.limitData[3].revert = ui->chInvCB_4->currentIndex();
    g_model.limitData[4].revert = ui->chInvCB_5->currentIndex();
    g_model.limitData[5].revert = ui->chInvCB_6->currentIndex();
    g_model.limitData[6].revert = ui->chInvCB_7->currentIndex();
    g_model.limitData[7].revert = ui->chInvCB_8->currentIndex();
    g_model.limitData[8].revert = ui->chInvCB_9->currentIndex();
    g_model.limitData[9].revert = ui->chInvCB_10->currentIndex();
    g_model.limitData[10].revert = ui->chInvCB_11->currentIndex();
    g_model.limitData[11].revert = ui->chInvCB_12->currentIndex();
    g_model.limitData[12].revert = ui->chInvCB_13->currentIndex();
    g_model.limitData[13].revert = ui->chInvCB_14->currentIndex();
    g_model.limitData[14].revert = ui->chInvCB_15->currentIndex();
    g_model.limitData[15].revert = ui->chInvCB_16->currentIndex();

    updateSettings();
}

void ModelEdit::curvePointEdited()
{            
    g_model.curves5[0][0] = ui->curvePt1_1->value();
    g_model.curves5[0][1] = ui->curvePt2_1->value();
    g_model.curves5[0][2] = ui->curvePt3_1->value();
    g_model.curves5[0][3] = ui->curvePt4_1->value();
    g_model.curves5[0][4] = ui->curvePt5_1->value();

    g_model.curves5[1][0] = ui->curvePt1_2->value();
    g_model.curves5[1][1] = ui->curvePt2_2->value();
    g_model.curves5[1][2] = ui->curvePt3_2->value();
    g_model.curves5[1][3] = ui->curvePt4_2->value();
    g_model.curves5[1][4] = ui->curvePt5_2->value();

    g_model.curves5[2][0] = ui->curvePt1_3->value();
    g_model.curves5[2][1] = ui->curvePt2_3->value();
    g_model.curves5[2][2] = ui->curvePt3_3->value();
    g_model.curves5[2][3] = ui->curvePt4_3->value();
    g_model.curves5[2][4] = ui->curvePt5_3->value();

    g_model.curves5[3][0] = ui->curvePt1_4->value();
    g_model.curves5[3][1] = ui->curvePt2_4->value();
    g_model.curves5[3][2] = ui->curvePt3_4->value();
    g_model.curves5[3][3] = ui->curvePt4_4->value();
    g_model.curves5[3][4] = ui->curvePt5_4->value();

    g_model.curves5[4][0] = ui->curvePt1_5->value();
    g_model.curves5[4][1] = ui->curvePt2_5->value();
    g_model.curves5[4][2] = ui->curvePt3_5->value();
    g_model.curves5[4][3] = ui->curvePt4_5->value();
    g_model.curves5[4][4] = ui->curvePt5_5->value();

    g_model.curves5[5][0] = ui->curvePt1_6->value();
    g_model.curves5[5][1] = ui->curvePt2_6->value();
    g_model.curves5[5][2] = ui->curvePt3_6->value();
    g_model.curves5[5][3] = ui->curvePt4_6->value();
    g_model.curves5[5][4] = ui->curvePt5_6->value();

    g_model.curves5[6][0] = ui->curvePt1_7->value();
    g_model.curves5[6][1] = ui->curvePt2_7->value();
    g_model.curves5[6][2] = ui->curvePt3_7->value();
    g_model.curves5[6][3] = ui->curvePt4_7->value();
    g_model.curves5[6][4] = ui->curvePt5_7->value();

    g_model.curves5[7][0] = ui->curvePt1_8->value();
    g_model.curves5[7][1] = ui->curvePt2_8->value();
    g_model.curves5[7][2] = ui->curvePt3_8->value();
    g_model.curves5[7][3] = ui->curvePt4_8->value();
    g_model.curves5[7][4] = ui->curvePt5_8->value();


    g_model.curves9[0][0] = ui->curvePt1_9->value();
    g_model.curves9[0][1] = ui->curvePt2_9->value();
    g_model.curves9[0][2] = ui->curvePt3_9->value();
    g_model.curves9[0][3] = ui->curvePt4_9->value();
    g_model.curves9[0][4] = ui->curvePt5_9->value();
    g_model.curves9[0][5] = ui->curvePt6_9->value();
    g_model.curves9[0][6] = ui->curvePt7_9->value();
    g_model.curves9[0][7] = ui->curvePt8_9->value();
    g_model.curves9[0][8] = ui->curvePt9_9->value();

    g_model.curves9[1][0] = ui->curvePt1_10->value();
    g_model.curves9[1][1] = ui->curvePt2_10->value();
    g_model.curves9[1][2] = ui->curvePt3_10->value();
    g_model.curves9[1][3] = ui->curvePt4_10->value();
    g_model.curves9[1][4] = ui->curvePt5_10->value();
    g_model.curves9[1][5] = ui->curvePt6_10->value();
    g_model.curves9[1][6] = ui->curvePt7_10->value();
    g_model.curves9[1][7] = ui->curvePt8_10->value();
    g_model.curves9[1][8] = ui->curvePt9_10->value();

    g_model.curves9[2][0] = ui->curvePt1_11->value();
    g_model.curves9[2][1] = ui->curvePt2_11->value();
    g_model.curves9[2][2] = ui->curvePt3_11->value();
    g_model.curves9[2][3] = ui->curvePt4_11->value();
    g_model.curves9[2][4] = ui->curvePt5_11->value();
    g_model.curves9[2][5] = ui->curvePt6_11->value();
    g_model.curves9[2][6] = ui->curvePt7_11->value();
    g_model.curves9[2][7] = ui->curvePt8_11->value();
    g_model.curves9[2][8] = ui->curvePt9_11->value();

    g_model.curves9[3][0] = ui->curvePt1_12->value();
    g_model.curves9[3][1] = ui->curvePt2_12->value();
    g_model.curves9[3][2] = ui->curvePt3_12->value();
    g_model.curves9[3][3] = ui->curvePt4_12->value();
    g_model.curves9[3][4] = ui->curvePt5_12->value();
    g_model.curves9[3][5] = ui->curvePt6_12->value();
    g_model.curves9[3][6] = ui->curvePt7_12->value();
    g_model.curves9[3][7] = ui->curvePt8_12->value();
    g_model.curves9[3][8] = ui->curvePt9_12->value();

    g_model.curves9[4][0] = ui->curvePt1_13->value();
    g_model.curves9[4][1] = ui->curvePt2_13->value();
    g_model.curves9[4][2] = ui->curvePt3_13->value();
    g_model.curves9[4][3] = ui->curvePt4_13->value();
    g_model.curves9[4][4] = ui->curvePt5_13->value();
    g_model.curves9[4][5] = ui->curvePt6_13->value();
    g_model.curves9[4][6] = ui->curvePt7_13->value();
    g_model.curves9[4][7] = ui->curvePt8_13->value();
    g_model.curves9[4][8] = ui->curvePt9_13->value();

    g_model.curves9[5][0] = ui->curvePt1_14->value();
    g_model.curves9[5][1] = ui->curvePt2_14->value();
    g_model.curves9[5][2] = ui->curvePt3_14->value();
    g_model.curves9[5][3] = ui->curvePt4_14->value();
    g_model.curves9[5][4] = ui->curvePt5_14->value();
    g_model.curves9[5][5] = ui->curvePt6_14->value();
    g_model.curves9[5][6] = ui->curvePt7_14->value();
    g_model.curves9[5][7] = ui->curvePt8_14->value();
    g_model.curves9[5][8] = ui->curvePt9_14->value();

    g_model.curves9[6][0] = ui->curvePt1_15->value();
    g_model.curves9[6][1] = ui->curvePt2_15->value();
    g_model.curves9[6][2] = ui->curvePt3_15->value();
    g_model.curves9[6][3] = ui->curvePt4_15->value();
    g_model.curves9[6][4] = ui->curvePt5_15->value();
    g_model.curves9[6][5] = ui->curvePt6_15->value();
    g_model.curves9[6][6] = ui->curvePt7_15->value();
    g_model.curves9[6][7] = ui->curvePt8_15->value();
    g_model.curves9[6][8] = ui->curvePt9_15->value();

    g_model.curves9[7][0] = ui->curvePt1_16->value();
    g_model.curves9[7][1] = ui->curvePt2_16->value();
    g_model.curves9[7][2] = ui->curvePt3_16->value();
    g_model.curves9[7][3] = ui->curvePt4_16->value();
    g_model.curves9[7][4] = ui->curvePt5_16->value();
    g_model.curves9[7][5] = ui->curvePt6_16->value();
    g_model.curves9[7][6] = ui->curvePt7_16->value();
    g_model.curves9[7][7] = ui->curvePt8_16->value();
    g_model.curves9[7][8] = ui->curvePt9_16->value();

    updateSettings();
}

void ModelEdit::tabSwitches()
{
    populateSourceCB(ui->cswitchCB_1,g_eeGeneral.stickMode,g_model.customSw[0].input);
    populateSourceCB(ui->cswitchCB_2,g_eeGeneral.stickMode,g_model.customSw[2].input);
    populateSourceCB(ui->cswitchCB_3,g_eeGeneral.stickMode,g_model.customSw[2].input);
    populateSourceCB(ui->cswitchCB_4,g_eeGeneral.stickMode,g_model.customSw[3].input);
    populateSourceCB(ui->cswitchCB_5,g_eeGeneral.stickMode,g_model.customSw[4].input);
    populateSourceCB(ui->cswitchCB_6,g_eeGeneral.stickMode,g_model.customSw[5].input);

    ui->cswitchOfs_1->setValue(g_model.customSw[0].offset);
    ui->cswitchOfs_2->setValue(g_model.customSw[1].offset);
    ui->cswitchOfs_3->setValue(g_model.customSw[2].offset);
    ui->cswitchOfs_4->setValue(g_model.customSw[3].offset);
    ui->cswitchOfs_5->setValue(g_model.customSw[4].offset);
    ui->cswitchOfs_6->setValue(g_model.customSw[5].offset);

    ui->cswitchFunc_1->setCurrentIndex(g_model.customSw[0].func);
    ui->cswitchFunc_2->setCurrentIndex(g_model.customSw[1].func);
    ui->cswitchFunc_3->setCurrentIndex(g_model.customSw[2].func);
    ui->cswitchFunc_4->setCurrentIndex(g_model.customSw[3].func);
    ui->cswitchFunc_5->setCurrentIndex(g_model.customSw[4].func);
    ui->cswitchFunc_6->setCurrentIndex(g_model.customSw[5].func);

    connect(ui->cswitchCB_1,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchCB_2,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchCB_3,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchCB_4,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchCB_5,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchCB_6,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));

    connect(ui->cswitchOfs_1,SIGNAL(editingFinished()),this,SLOT(switchesEdited()));
    connect(ui->cswitchOfs_2,SIGNAL(editingFinished()),this,SLOT(switchesEdited()));
    connect(ui->cswitchOfs_3,SIGNAL(editingFinished()),this,SLOT(switchesEdited()));
    connect(ui->cswitchOfs_4,SIGNAL(editingFinished()),this,SLOT(switchesEdited()));
    connect(ui->cswitchOfs_5,SIGNAL(editingFinished()),this,SLOT(switchesEdited()));
    connect(ui->cswitchOfs_6,SIGNAL(editingFinished()),this,SLOT(switchesEdited()));

    connect(ui->cswitchFunc_1,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchFunc_2,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchFunc_3,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchFunc_4,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchFunc_5,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
    connect(ui->cswitchFunc_6,SIGNAL(currentIndexChanged(int)),this,SLOT(switchesEdited()));
}

void ModelEdit::switchesEdited()
{
    g_model.customSw[0].input = ui->cswitchCB_1->currentIndex();
    g_model.customSw[1].input = ui->cswitchCB_2->currentIndex();
    g_model.customSw[2].input = ui->cswitchCB_3->currentIndex();
    g_model.customSw[3].input = ui->cswitchCB_4->currentIndex();
    g_model.customSw[4].input = ui->cswitchCB_5->currentIndex();
    g_model.customSw[5].input = ui->cswitchCB_6->currentIndex();

    g_model.customSw[0].offset = ui->cswitchOfs_1->value();
    g_model.customSw[1].offset = ui->cswitchOfs_2->value();
    g_model.customSw[2].offset = ui->cswitchOfs_3->value();
    g_model.customSw[3].offset = ui->cswitchOfs_4->value();
    g_model.customSw[4].offset = ui->cswitchOfs_5->value();
    g_model.customSw[5].offset = ui->cswitchOfs_6->value();

    g_model.customSw[0].func = ui->cswitchFunc_1->currentIndex();
    g_model.customSw[1].func = ui->cswitchFunc_2->currentIndex();
    g_model.customSw[2].func = ui->cswitchFunc_3->currentIndex();
    g_model.customSw[3].func = ui->cswitchFunc_4->currentIndex();
    g_model.customSw[4].func = ui->cswitchFunc_5->currentIndex();
    g_model.customSw[5].func = ui->cswitchFunc_6->currentIndex();

    updateSettings();
}

void ModelEdit::tabTrims()
{
    ui->spinBox_S1->setValue(g_model.trim[CONVERT_MODE(RUD)-1]);
    ui->spinBox_S2->setValue(g_model.trim[CONVERT_MODE(ELE)-1]);
    ui->spinBox_S3->setValue(g_model.trim[CONVERT_MODE(THR)-1]);
    ui->spinBox_S4->setValue(g_model.trim[CONVERT_MODE(AIL)-1]);


    switch (g_eeGeneral.stickMode)
    {
        case (0):
            ui->Label_S1->setText("RUD");
            ui->Label_S2->setText("ELE");
            ui->Label_S3->setText("THR");
            ui->Label_S4->setText("AIL");
            break;
        case (1):
            ui->Label_S1->setText("RUD");
            ui->Label_S2->setText("THR");
            ui->Label_S3->setText("ELE");
            ui->Label_S4->setText("AIL");
            break;
        case (2):
            ui->Label_S1->setText("AIL");
            ui->Label_S2->setText("ELE");
            ui->Label_S3->setText("THR");
            ui->Label_S4->setText("RUD");
            break;
        case (3):
            ui->Label_S1->setText("AIL");
            ui->Label_S2->setText("THR");
            ui->Label_S3->setText("ELE");
            ui->Label_S4->setText("RUD");
            break;
    }

}


void ModelEdit::on_modelNameLE_textEdited(QString txt)
{
    //allow uppercase, number, ' ', '.', '_', '-' only
    ui->modelNameLE->setText(txt.toUpper());
}

void ModelEdit::on_modelNameLE_editingFinished()
{
    uint8_t temp = g_model.mdVers;
    memset(&g_model.name,' ',sizeof(g_model.name));
    const char *c = ui->modelNameLE->text().left(10).toAscii();
    strcpy((char*)&g_model.name,c);
    g_model.mdVers = temp;  //in case strcpy overruns
    for(int i=0; i<10; i++) if(!g_model.name[i]) g_model.name[i] = ' ';
    updateSettings();

}

void ModelEdit::on_timerModeCB_currentIndexChanged(int index)
{
    g_model.tmrMode = index-TMR_NUM_OPTION;
    updateSettings();
}

void ModelEdit::on_timerDirCB_currentIndexChanged(int index)
{
    g_model.tmrDir = index;
    updateSettings();
}

void ModelEdit::on_trimIncCB_currentIndexChanged(int index)
{
    g_model.trimInc = index;
    updateSettings();
}

void ModelEdit::on_trimSWCB_currentIndexChanged(int index)
{
    g_model.trimSw = index-MAX_DRSWITCH;
    updateSettings();
}

void ModelEdit::on_pulsePolCB_currentIndexChanged(int index)
{
    g_model.pulsePol = index;
    updateSettings();
}

void ModelEdit::on_protocolCB_currentIndexChanged(int index)
{
    g_model.protocol = index;
    updateSettings();

    ui->ppmDelaySB->setEnabled(!g_model.protocol);
    ui->numChannelsSB->setEnabled(!g_model.protocol);
}

void ModelEdit::on_timerValTE_editingFinished()
{
    g_model.tmrVal = ui->timerValTE->time().minute()*60 + ui->timerValTE->time().second();
    updateSettings();
}

void ModelEdit::on_numChannelsSB_editingFinished()
{
    int i = (ui->numChannelsSB->value()-8)/2;
    if((i*2+8)!=ui->numChannelsSB->value()) ui->numChannelsSB->setValue(i*2+8);
    g_model.ppmNCH = i;
    updateSettings();
}

void ModelEdit::on_ppmDelaySB_editingFinished()
{
    int i = (ui->ppmDelaySB->value()-300)/50;
    if((i*50+300)!=ui->ppmDelaySB->value()) ui->ppmDelaySB->setValue(i*50+300);
    g_model.ppmDelay = i;
    updateSettings();
}


void ModelEdit::on_thrTrimChkB_toggled(bool checked)
{
    g_model.thrTrim = checked;
    updateSettings();
}

void ModelEdit::on_thrExpoChkB_toggled(bool checked)
{
    g_model.thrExpo = checked;
    updateSettings();
}

void ModelEdit::on_bcRUDChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_RUD;
    else
        g_model.beepANACenter &= ~BC_BIT_RUD;
    updateSettings();
}

void ModelEdit::on_bcELEChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_ELE;
    else
        g_model.beepANACenter &= ~BC_BIT_ELE;
    updateSettings();
}

void ModelEdit::on_bcTHRChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_THR;
    else
        g_model.beepANACenter &= ~BC_BIT_THR;
    updateSettings();
}

void ModelEdit::on_bcAILChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_AIL;
    else
        g_model.beepANACenter &= ~BC_BIT_AIL;
    updateSettings();
}

void ModelEdit::on_bcP1ChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_P1;
    else
        g_model.beepANACenter &= ~BC_BIT_P1;
    updateSettings();
}

void ModelEdit::on_bcP2ChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_P2;
    else
        g_model.beepANACenter &= ~BC_BIT_P2;
    updateSettings();
}

void ModelEdit::on_bcP3ChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_P3;
    else
        g_model.beepANACenter &= ~BC_BIT_P3;
    updateSettings();
}


void ModelEdit::on_spinBox_S1_valueChanged(int value)
{
        g_model.trim[CONVERT_MODE(RUD)-1] = value;
        updateSettings();
}

void ModelEdit::on_spinBox_S2_valueChanged(int value)
{
        g_model.trim[CONVERT_MODE(ELE)-1] = value;
        updateSettings();
}

void ModelEdit::on_spinBox_S3_valueChanged(int value)
{
        g_model.trim[CONVERT_MODE(THR)-1] = value;
        updateSettings();
}

void ModelEdit::on_spinBox_S4_valueChanged(int value)
{
        g_model.trim[CONVERT_MODE(AIL)-1] = value;
        updateSettings();
}




void ModelEdit::curveEdit_clicked(int crv)
{
    if(crv<1 || crv>16) return;

    Node *nodel = 0;
    Node *nodex = 0;

    QGraphicsScene *scene = ui->curvePreview->scene();
    scene->clear();

    //scene->addRect(-125,-125,250,250);


    QGraphicsSimpleTextItem *ti;
    ti = scene->addSimpleText(tr("Curve %1").arg(crv));
    ti->setPos(0,0);



    if(crv<=8)
        for(int i=0; i<5; i++)
        {
            nodel = nodex;
            nodex = new Node(ui->curvePreview);

            nodex->setPos(i*30,g_model.curves5[crv-1][i]);
            scene->addItem(nodex);
            if(i>0) scene->addItem(new Edge(nodel, nodex));
        }
    else
        for(int i=0; i<9; i++)
        {
            nodel = nodex;
            nodex = new Node(ui->curvePreview);

            nodex->setPos(i*30,g_model.curves9[crv-9][i]);
            scene->addItem(nodex);
            if(i>0) scene->addItem(new Edge(nodel, nodex));
        }
}




void ModelEdit::on_curveEdit_1_clicked()
{
    curveEdit_clicked(1);
}

void ModelEdit::on_curveEdit_2_clicked()
{
    curveEdit_clicked(2);
}

void ModelEdit::on_curveEdit_3_clicked()
{
    curveEdit_clicked(3);
}

void ModelEdit::on_curveEdit_4_clicked()
{
    curveEdit_clicked(4);
}

void ModelEdit::on_curveEdit_5_clicked()
{
    curveEdit_clicked(5);
}

void ModelEdit::on_curveEdit_6_clicked()
{
    curveEdit_clicked(6);
}

void ModelEdit::on_curveEdit_7_clicked()
{
    curveEdit_clicked(7);
}

void ModelEdit::on_curveEdit_8_clicked()
{
    curveEdit_clicked(8);
}

void ModelEdit::on_curveEdit_9_clicked()
{
    curveEdit_clicked(9);
}

void ModelEdit::on_curveEdit_10_clicked()
{
    curveEdit_clicked(10);
}

void ModelEdit::on_curveEdit_11_clicked()
{
    curveEdit_clicked(11);
}

void ModelEdit::on_curveEdit_12_clicked()
{
    curveEdit_clicked(12);
}

void ModelEdit::on_curveEdit_13_clicked()
{
    curveEdit_clicked(13);
}

void ModelEdit::on_curveEdit_14_clicked()
{
    curveEdit_clicked(14);
}

void ModelEdit::on_curveEdit_15_clicked()
{
    curveEdit_clicked(15);
}

void ModelEdit::on_curveEdit_16_clicked()
{
    curveEdit_clicked(16);
}
