/** Header file generated with fdesign on Wed May 24 12:49:51 2000.**/

#ifndef FD_ImpFeMain_h_
#define FD_ImpFeMain_h_

/** Callbacks, globals and object handlers **/
extern void HdnRtn_cb(FL_OBJECT *, long);
extern void MainInputBox_cb(FL_OBJECT *, long);
extern void MainHelpMenu_cb(FL_OBJECT *, long);
extern void PrivMsgBtn_cb(FL_OBJECT *, long);
extern void MainFileMenu_cb(FL_OBJECT *, long);
extern void MainServerMenu_cb(FL_OBJECT *, long);
extern void PrivMsgWho_cb(FL_OBJECT *, long);
extern void MainViewMenu_cb(FL_OBJECT *, long);
extern void MainConfigMenu_cb(FL_OBJECT *, long);

extern void AboutOk_cb(FL_OBJECT *, long);

extern void MB_cb(FL_OBJECT *, long);
extern void MapHelpMenu_cb(FL_OBJECT *, long);
extern void MapFileMenu_cb(FL_OBJECT *, long);
extern void DirBtn_cb(FL_OBJECT *, long);
extern void dummy_cb(FL_OBJECT *, long);
extern void JmpBtn_cb(FL_OBJECT *, long);
extern void MapOk_cb(FL_OBJECT *, long);

extern void TasksHelpMenu_cb(FL_OBJECT *, long);
extern void TasksTasksMenu_cb(FL_OBJECT *, long);
extern void TasksUpdtBtn_cb(FL_OBJECT *, long);
extern void TasksKillBtn_cb(FL_OBJECT *, long);
extern void TasksDoneBtn_cb(FL_OBJECT *, long);

extern void ServName_cb(FL_OBJECT *, long);
extern void ServPort_cb(FL_OBJECT *, long);
extern void PlayerName_cb(FL_OBJECT *, long);
extern void PlayerPswd_cb(FL_OBJECT *, long);
extern void GameDesc_cb(FL_OBJECT *, long);
extern void ServCfgOkBtn_cb(FL_OBJECT *, long);
extern void ServCfgCancelBtn_cb(FL_OBJECT *, long);

extern void ShBrowse_cb(FL_OBJECT *, long);
extern void ShCenUpdate_cb(FL_OBJECT *, long);
extern void ShCenOk_cb(FL_OBJECT *, long);

extern void PlBrowse_cb(FL_OBJECT *, long);
extern void PlCenUpdate_cb(FL_OBJECT *, long);
extern void PlCenOk_cb(FL_OBJECT *, long);

extern void dummy_cb(FL_OBJECT *, long);
extern void ShDetUpdBtn_cb(FL_OBJECT *, long);
extern void ShDetStatBtn_cb(FL_OBJECT *, long);
extern void ShDetCargoBtn_cb(FL_OBJECT *, long);
extern void ShDetBigBtn_cb(FL_OBJECT *, long);
extern void ShDetActionBtn_cb(FL_OBJECT *, long);
extern void ShDetOk_cb(FL_OBJECT *, long);

extern void dummy_cb(FL_OBJECT *, long);
extern void NameUpdate_cb(FL_OBJECT *, long);
extern void CheckpointUpdate_cb(FL_OBJECT *, long);
extern void PlDetActionBtn_cb(FL_OBJECT *, long);
extern void PlDetUpdBtn_cb(FL_OBJECT *, long);
extern void PlDetGeoBtn_cb(FL_OBJECT *, long);
extern void PlDetPopBtn_cb(FL_OBJECT *, long);
extern void PlDetBigBtn_cb(FL_OBJECT *, long);
extern void PlDetProdBtn_cb(FL_OBJECT *, long);
extern void PlDetDumpBtn_cb(FL_OBJECT *, long);
extern void PlDetOk_cb(FL_OBJECT *, long);










extern void dummy_cb(FL_OBJECT *, long);
extern void HelpOk_cb(FL_OBJECT *, long);

extern void CmdsHelpMenu_cb(FL_OBJECT *, long);
extern void CmdsBrowser_cb(FL_OBJECT *, long);
extern void CmdsKillBtn_cb(FL_OBJECT *, long);
extern void CmdsDoneBtn_cb(FL_OBJECT *, long);
extern void CmdsPauseBtn_cb(FL_OBJECT *, long);
extern void CmdsAddBtn_cb(FL_OBJECT *, long);
extern void dummy_cb(FL_OBJECT *, long);
extern void CmdsModBtn_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *ImpFeMain;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *HdnRtn;
	FL_OBJECT *MainInputBox;
	FL_OBJECT *MainHelpMenu;
	FL_OBJECT *MainBrowser;
	FL_OBJECT *PrivMsgBtn;
	FL_OBJECT *MainFileMenu;
	FL_OBJECT *MainServerMenu;
	FL_OBJECT *PrivMsgWho;
	FL_OBJECT *MainViewMenu;
	FL_OBJECT *MainConfigMenu;
	FL_OBJECT *MainPrompt;
	FL_OBJECT *BtuDisp;
	FL_OBJECT *TimeDisp;
	FL_OBJECT *StatusBox;
	FL_OBJECT *QueueDisp;
	FL_OBJECT *ScriptDisp;
} FD_ImpFeMain;

extern FD_ImpFeMain * create_form_ImpFeMain(void);
typedef struct {
	FL_FORM *ImpFeAbout;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *AboutOkButn;
	FL_OBJECT *MainAboutText;
	FL_OBJECT *ImpVer;
	FL_OBJECT *ImpLogo;
} FD_ImpFeAbout;

extern FD_ImpFeAbout * create_form_ImpFeAbout(void);
typedef struct {
	FL_FORM *MapForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *AA;
	FL_OBJECT *MapHelpMenu;
	FL_OBJECT *MapFileMenu;
	FL_OBJECT *UpBtn;
	FL_OBJECT *DnBtn;
	FL_OBJECT *RtBtn;
	FL_OBJECT *LtBtn;
	FL_OBJECT *DRBtn;
	FL_OBJECT *URBtn;
	FL_OBJECT *ULBtn;
	FL_OBJECT *DLBtn;
	FL_OBJECT *BigUpBtn;
	FL_OBJECT *BigRtBtn;
	FL_OBJECT *BigDnBtn;
	FL_OBJECT *BigLtBtn;
	FL_OBJECT *BigURBtn;
	FL_OBJECT *BigDRBtn;
	FL_OBJECT *BigULBtn;
	FL_OBJECT *BigDLBtn;
	FL_OBJECT *MapCellGroup;
	FL_OBJECT *A1;
	FL_OBJECT *B1;
	FL_OBJECT *C1;
	FL_OBJECT *D1;
	FL_OBJECT *E1;
	FL_OBJECT *F1;
	FL_OBJECT *G1;
	FL_OBJECT *A2;
	FL_OBJECT *A3;
	FL_OBJECT *A4;
	FL_OBJECT *A5;
	FL_OBJECT *A6;
	FL_OBJECT *A7;
	FL_OBJECT *A8;
	FL_OBJECT *A9;
	FL_OBJECT *B2;
	FL_OBJECT *B3;
	FL_OBJECT *B4;
	FL_OBJECT *B5;
	FL_OBJECT *B6;
	FL_OBJECT *B8;
	FL_OBJECT *B9;
	FL_OBJECT *B7;
	FL_OBJECT *C2;
	FL_OBJECT *C3;
	FL_OBJECT *C4;
	FL_OBJECT *C5;
	FL_OBJECT *C6;
	FL_OBJECT *C7;
	FL_OBJECT *C8;
	FL_OBJECT *C9;
	FL_OBJECT *D2;
	FL_OBJECT *D3;
	FL_OBJECT *D4;
	FL_OBJECT *D5;
	FL_OBJECT *D6;
	FL_OBJECT *D7;
	FL_OBJECT *D8;
	FL_OBJECT *D9;
	FL_OBJECT *E2;
	FL_OBJECT *E3;
	FL_OBJECT *E4;
	FL_OBJECT *E5;
	FL_OBJECT *E6;
	FL_OBJECT *E7;
	FL_OBJECT *E8;
	FL_OBJECT *E9;
	FL_OBJECT *F2;
	FL_OBJECT *F3;
	FL_OBJECT *F4;
	FL_OBJECT *F5;
	FL_OBJECT *F6;
	FL_OBJECT *F7;
	FL_OBJECT *F8;
	FL_OBJECT *F9;
	FL_OBJECT *G2;
	FL_OBJECT *G3;
	FL_OBJECT *G4;
	FL_OBJECT *G5;
	FL_OBJECT *G6;
	FL_OBJECT *G7;
	FL_OBJECT *G8;
	FL_OBJECT *G9;
	FL_OBJECT *BA;
	FL_OBJECT *CA;
	FL_OBJECT *DA;
	FL_OBJECT *EA;
	FL_OBJECT *FA;
	FL_OBJECT *GA;
	FL_OBJECT *Col1;
	FL_OBJECT *Col2;
	FL_OBJECT *Col3;
	FL_OBJECT *Col4;
	FL_OBJECT *Col5;
	FL_OBJECT *Col6;
	FL_OBJECT *Col7;
	FL_OBJECT *Col8;
	FL_OBJECT *Col9;
	FL_OBJECT *ColA;
	FL_OBJECT *Row1;
	FL_OBJECT *Row2;
	FL_OBJECT *Row3;
	FL_OBJECT *Row4;
	FL_OBJECT *Row5;
	FL_OBJECT *Row6;
	FL_OBJECT *Row7;
	FL_OBJECT *JmRow;
	FL_OBJECT *JmCol;
	FL_OBJECT *JmpBtn;
	FL_OBJECT *HomeBtn;
	FL_OBJECT *MapOkButn;
} FD_MapForm;

extern FD_MapForm * create_form_MapForm(void);
typedef struct {
	FL_FORM *TasksForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *TasksHelpMenu;
	FL_OBJECT *TasksTasksMenu;
	FL_OBJECT *TasksBrowser;
	FL_OBJECT *TasksBtns;
	FL_OBJECT *TasksUpdtBtn;
	FL_OBJECT *TasksKillBtn;
	FL_OBJECT *TasksDoneBtn;
} FD_TasksForm;

extern FD_TasksForm * create_form_TasksForm(void);
typedef struct {
	FL_FORM *ServCfgForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *ServName;
	FL_OBJECT *ServPort;
	FL_OBJECT *PlayerName;
	FL_OBJECT *PlayerPswd;
	FL_OBJECT *GameDesc;
	FL_OBJECT *ServerDataGroup;
	FL_OBJECT *ServCfgOkBtn;
	FL_OBJECT *ServCfgCancelBtn;
} FD_ServCfgForm;

extern FD_ServCfgForm * create_form_ServCfgForm(void);
typedef struct {
	FL_FORM *ShipCensusForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *ShBrowse;
	FL_OBJECT *Update;
	FL_OBJECT *ShCenOkButn;
} FD_ShipCensusForm;

extern FD_ShipCensusForm * create_form_ShipCensusForm(void);
typedef struct {
	FL_FORM *PlanetCensusForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *PlBrowse;
	FL_OBJECT *Update;
	FL_OBJECT *PlCenOkButn;
} FD_PlanetCensusForm;

extern FD_PlanetCensusForm * create_form_PlanetCensusForm(void);
typedef struct {
	FL_FORM *ShipDetailForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *ShNum;
	FL_OBJECT *Fuel;
	FL_OBJECT *Armor;
	FL_OBJECT *Energy;
	FL_OBJECT *Effic;
	FL_OBJECT *Shields;
	FL_OBJECT *ShKeep;
	FL_OBJECT *Air;
	FL_OBJECT *Owner;
	FL_OBJECT *RowCol;
	FL_OBJECT *TF;
	FL_OBJECT *Plague;
	FL_OBJECT *HullTF;
	FL_OBJECT *EngTF;
	FL_OBJECT *EngEff;
	FL_OBJECT *Civ;
	FL_OBJECT *Mil;
	FL_OBJECT *Sci;
	FL_OBJECT *Ofc;
	FL_OBJECT *ShDetItem;
	FL_OBJECT *Torp;
	FL_OBJECT *Ore;
	FL_OBJECT *Bars;
	FL_OBJECT *AirTnk;
	FL_OBJECT *FuelTnk;
	FL_OBJECT *Cargo;
	FL_OBJECT *Seen;
	FL_OBJECT *ShDetUpdBtn;
	FL_OBJECT *ShDetStatBtn;
	FL_OBJECT *ShDetCargoBtn;
	FL_OBJECT *ShDetBigBtn;
	FL_OBJECT *ShDetInstallBtn;
	FL_OBJECT *ShDetUninstallBtn;
	FL_OBJECT *ShDetUnloadBtn;
	FL_OBJECT *Where;
	FL_OBJECT *ShDetOkButn;
} FD_ShipDetailForm;

extern FD_ShipDetailForm * create_form_ShipDetailForm(void);
typedef struct {
	FL_FORM *PlanetDetailForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *Name;
	FL_OBJECT *PlNum;
	FL_OBJECT *Owner;
	FL_OBJECT *RowCol;
	FL_OBJECT *Seen;
	FL_OBJECT *Civ;
	FL_OBJECT *Mil;
	FL_OBJECT *Sci;
	FL_OBJECT *Ofc;
	FL_OBJECT *Misl;
	FL_OBJECT *Ore;
	FL_OBJECT *AirTnk;
	FL_OBJECT *FuelTnk;
	FL_OBJECT *Plane;
	FL_OBJECT *Bars;
	FL_OBJECT *PlProdBrowse;
	FL_OBJECT *Size;
	FL_OBJECT *Effic;
	FL_OBJECT *Class;
	FL_OBJECT *Polut;
	FL_OBJECT *Minerals;
	FL_OBJECT *Gold;
	FL_OBJECT *Water;
	FL_OBJECT *Gas;
	FL_OBJECT *Mobil;
	FL_OBJECT *Plague;
	FL_OBJECT *Tech;
	FL_OBJECT *Research;
	FL_OBJECT *TF;
	FL_OBJECT *RF;
	FL_OBJECT *PrevOwn;
	FL_OBJECT *Transfer;
	FL_OBJECT *OwnerRel;
	FL_OBJECT *Checkpoint;
	FL_OBJECT *NameUpdate;
	FL_OBJECT *CheckpointUpdate;
	FL_OBJECT *BTU;
	FL_OBJECT *PlDetItem;
	FL_OBJECT *PlDetRefurbBtn;
	FL_OBJECT *PlDetUpdBtn;
	FL_OBJECT *PlDetGeoBtn;
	FL_OBJECT *PlDetPopBtn;
	FL_OBJECT *PlDetBigBtn;
	FL_OBJECT *PF;
	FL_OBJECT *PopPct;
	FL_OBJECT *PlDetProdBtn;
	FL_OBJECT *PlDetDumpBtn;
	FL_OBJECT *PlDetOkButn;
} FD_PlanetDetailForm;

extern FD_PlanetDetailForm * create_form_PlanetDetailForm(void);
typedef struct {
	FL_FORM *PowerRptForm;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_PowerRptForm;

extern FD_PowerRptForm * create_form_PowerRptForm(void);
typedef struct {
	FL_FORM *MinerCensusForm;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_MinerCensusForm;

extern FD_MinerCensusForm * create_form_MinerCensusForm(void);
typedef struct {
	FL_FORM *DebugOutputForm;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_DebugOutputForm;

extern FD_DebugOutputForm * create_form_DebugOutputForm(void);
typedef struct {
	FL_FORM *LocalEditorForm;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_LocalEditorForm;

extern FD_LocalEditorForm * create_form_LocalEditorForm(void);
typedef struct {
	FL_FORM *Dummy1Form;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_Dummy1Form;

extern FD_Dummy1Form * create_form_Dummy1Form(void);
typedef struct {
	FL_FORM *Dummy2Form;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_Dummy2Form;

extern FD_Dummy2Form * create_form_Dummy2Form(void);
typedef struct {
	FL_FORM *Dummy3Form;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_Dummy3Form;

extern FD_Dummy3Form * create_form_Dummy3Form(void);
typedef struct {
	FL_FORM *Dummy4Form;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_Dummy4Form;

extern FD_Dummy4Form * create_form_Dummy4Form(void);
typedef struct {
	FL_FORM *Dummy5Form;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_Dummy5Form;

extern FD_Dummy5Form * create_form_Dummy5Form(void);
typedef struct {
	FL_FORM *HelpForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *Help_Browser;
	FL_OBJECT *OkBtn;
} FD_HelpForm;

extern FD_HelpForm * create_form_HelpForm(void);
typedef struct {
	FL_FORM *CommandForm;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *CmdsHelpMenu;
	FL_OBJECT *CmdsBrowser;
	FL_OBJECT *CmdsKillBtn;
	FL_OBJECT *CmdsDoneBtn;
	FL_OBJECT *CmdsPauseBtn;
	FL_OBJECT *CmdsAddBtn;
	FL_OBJECT *CmdsCmd;
	FL_OBJECT *CmdsPriorBtn;
	FL_OBJECT *CmdsModBtn;
} FD_CommandForm;

extern FD_CommandForm * create_form_CommandForm(void);

#endif /* FD_ImpFeMain_h_ */
