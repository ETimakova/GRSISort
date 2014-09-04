
#include "TAnalysisTreeBuilder.h"
#include "TGRSIOptions.h"

bool TEventQueue::lock = false;
std::queue<std::vector<TFragment>*> TEventQueue::fEventQueue;
TEventQueue *TEventQueue::fPtrToQue = 0;

TEventQueue::TEventQueue() { }

TEventQueue::~TEventQueue() { }

TEventQueue *TEventQueue::Get() {
   if(!fPtrToQue)
      fPtrToQue = new TEventQueue;
   return fPtrToQue;
}

void TEventQueue::Add(std::vector<TFragment> *event) {
   while(lock) { }
   SetLock();
   fEventQueue.push(event);
   UnsetLock();
   return;
}

std::vector<TFragment> *TEventQueue::Pop() {
   std::vector<TFragment> *temp;
   while(lock) { }
   SetLock();
   temp = fEventQueue.front();
   fEventQueue.pop();
   UnsetLock();
   return temp;
}

int TEventQueue::Size() {
   int temp;
   while(lock) { }
   SetLock();
   temp = fEventQueue.size();
   UnsetLock();
   return temp;
}

///************************************************///
///************************************************///
///************************************************///
///************************************************///
///************************************************///
///************************************************///
///************************************************///
///************************************************///


TChain *TAnalysisTreeBuilder::fFragmentChain = 0;
TTree  *TAnalysisTreeBuilder::fCurrentFragTree = 0;
TFile  *TAnalysisTreeBuilder::fCurrentFragFile = 0;
TTree  *TAnalysisTreeBuilder::fCurrentAnalysisTree = 0;
TFile  *TAnalysisTreeBuilder::fCurrentAnalysisFile = 0;
TGRSIRunInfo *TAnalysisTreeBuilder::fCurrentRunInfo = 0;

TFragment *TAnalysisTreeBuilder::fragment = 0;

//TTigress    *TAnalysisTreeBuilder::tigress = 0;
TSharc      *TAnalysisTreeBuilder::sharc   = 0;  
//TTriFoil    *TAnalysisTreeBuilder::triFoil = 0;
//TRf         *TAnalysisTreeBuilder::rf      = 0;     
//TCSM        *TAnalysisTreeBuilder::csm     = 0;    
//TSpice      *TAnalysisTreeBuilder::spice   = 0;  
//TS3         *TAnalysisTreeBuilder::s3      = 0;
//TTip        *TAnalysisTreeBuilder::tip     = 0;    

//TGriffin    *TAnalysisTreeBuilder::Griffin = 0;
//TSceptar    *TAnalysisTreeBuilder::Sceptar = 0;
//TPaces      *TAnalysisTreeBuilder::Paces   = 0;  
//TDante      *TAnalysisTreeBuilder::Dante   = 0;  
//TZeroDegree *TAnalysisTreeBuilder::ZeroDegree = 0;
//TDescant    *TAnalysisTreeBuilder::Descant = 0;




TAnalysisTreeBuilder::TAnalysisTreeBuilder() { }

TAnalysisTreeBuilder::~TAnalysisTreeBuilder() { }

void TAnalysisTreeBuilder::StartMakeAnalysisTree(int argc, char** argv) {
   if(argc==1) {
      SetUpFragmentChain(TGRSIOptions::GetInputRoot());
   } else {
      return;
   }
   SortFragmentChain();   

}


void TAnalysisTreeBuilder::InitChannels() {
   
   if(!fCurrentFragTree)
      return;

   TChannel::DeleteAllChannels(); 
   TList *list = fCurrentFragTree->GetUserInfo();
   TIter iter(list);
   while(TObject *obj = iter.Next()) {
      if(!obj->InheritsFrom("TChannel"))
         continue;
      TChannel *chan = (TChannel*)obj;
      int number = chan->GetUserInfoNumber(); // I should need to do this.. 
      chan->SetNumber(number);
      TChannel::AddChannel(chan,"save");
      //chan->Print();
   }
   if(!TGRSIOptions::GetInputCal().empty()) {
      TChannel::ReadCalFile(TGRSIOptions::GetInputCal().at(0).c_str());
   }
   printf("AnalysisTreeBuilder:  read in %i TChannels.\n", TChannel::GetNumberOfChannels());
}  



void TAnalysisTreeBuilder::SetUpFragmentChain(std::vector<std::string> infiles) {
   TChain *chain = new TChain("FragmentTree");
   for(int x=0;x<infiles.size();x++) 
      chain->Add(infiles.at(x).c_str());
   SetUpFragmentChain(chain);
   
}

void TAnalysisTreeBuilder::SetUpFragmentChain(TChain *chain) {
   if(fFragmentChain)
      delete fFragmentChain;
   fFragmentChain = chain;
 
}

void TAnalysisTreeBuilder::SortFragmentChain() {
   if(!fFragmentChain)
      return;
   
   int ntrees = fFragmentChain->GetNtrees();
   int nChainEntries = fFragmentChain->GetEntries();
   int treeNumber, lastTreeNumber = -1;

   printf("Found %i trees with %i total fragments.\n",ntrees,nChainEntries);

   for(int i=0;i<nChainEntries;i++) {
      fFragmentChain->LoadTree(i);
      treeNumber = fFragmentChain->GetTreeNumber();
      if(treeNumber != lastTreeNumber) {
         if(lastTreeNumber == -1) {
            printf(DYELLOW "Sorting tree[%d]: %s" RESET_COLOR "\n",treeNumber+1,fFragmentChain->GetTree()->GetCurrentFile()->GetName());
         } else {
            printf(DYELLOW "Changing to tree[%d/%d]: %s" RESET_COLOR "\n",treeNumber+1,ntrees,fFragmentChain->GetTree()->GetCurrentFile()->GetName());
            printf(DYELLOW "    Switched from tree[%d] at chain entry number %i" RESET_COLOR "\n",lastTreeNumber,i);
         }
         lastTreeNumber = treeNumber;
      }  else {
         continue;
      }
      fCurrentFragTree = fFragmentChain->GetTree();
      int nentries = fCurrentFragTree->GetEntries();
      SetupFragmentTree();
      SetupOutFile();
      SetupAnalysisTree();
      SortFragmentTree();
      CloseAnalysisFile();
      printf("\n");
      i+=(nentries-10);
   }
   printf("Finished chain sort.\n");
   return;
}

void TAnalysisTreeBuilder::SortFragmentTree() {




   return;
}


void TAnalysisTreeBuilder::SetupFragmentTree() {

   InitChannels();
   fCurrentFragFile = fCurrentFragTree->GetCurrentFile();
   fCurrentRunInfo  = (TGRSIRunInfo*)fCurrentFragFile->Get("TGRSIRunInfo");
   if(fCurrentRunInfo) {
      TGRSIRunInfo::SetInfoFromFile(fCurrentRunInfo);
      fCurrentRunInfo->Print();
   }

   if(!fCurrentFragTree->GetTreeIndex()) {
      if(fCurrentRunInfo->MajorIndex().length()>0) {
         printf(DBLUE "Tree Index not found, building index on %s/%s..." RESET_COLOR,
                        fCurrentRunInfo->MajorIndex().c_str(),fCurrentRunInfo->MinorIndex().c_str());  fflush(stdout); 
         if(fCurrentRunInfo->MinorIndex().length()>0) {
            fCurrentFragTree->BuildIndex(fCurrentRunInfo->MajorIndex().c_str(),fCurrentRunInfo->MinorIndex().c_str());
         } else {
            fCurrentFragTree->BuildIndex(fCurrentRunInfo->MajorIndex().c_str());
         }
      } else {
         printf(DBLUE "Tree Index not found, building index on TriggerId/FragmentId..." RESET_COLOR);  fflush(stdout);   
         fCurrentFragTree->BuildIndex("TriggerId","FragmentId");
      }
      printf(DBLUE " done!" RESET_COLOR "\n");
   }



   return;
}

void TAnalysisTreeBuilder::SetupOutFile() {
   if(!fCurrentRunInfo)
      return;
   std::string outfilename = Form("analysis%05i_%03i.root",fCurrentRunInfo->RunNumber(),fCurrentRunInfo->SubRunNumber());
   if(fCurrentAnalysisFile)
      delete fCurrentAnalysisFile;
   fCurrentAnalysisFile = new TFile(outfilename.c_str(),"recreate");
   printf("created ouput file: %s\n",fCurrentAnalysisFile->GetName());
   return;
}

void TAnalysisTreeBuilder::SetupAnalysisTree() { 
   if(!fCurrentAnalysisFile || !fCurrentRunInfo)
      return;
   fCurrentAnalysisFile->cd();
   if(fCurrentAnalysisTree)
      delete fCurrentAnalysisTree;
   fCurrentAnalysisTree = new TTree("AnalysisTree","AnalysisTree");

   TGRSIRunInfo *info = fCurrentRunInfo;
   TTree *tree = fCurrentAnalysisTree;

   //if(info->Tigress())   { tree->Branch("TTigress","TTigress",&tigress); } 
   if(info->Sharc())     { tree->Branch("TSharc","TSharc",&sharc); } 
   //if(info->TriFoil())   { tree->Branch("TTriFoil","TTriFoil",&trifoil); } 
   //if(info->Rf())        { tree->Branch("TRf","TRf",&rf); } 
   //if(info->CSM())       { tree->Branch("TCSM","TCSM",&csm); } 
   //if(info->Spice())     { tree->Branch("TSpice","TSpice",&csm); tree->SetBranch("TS3","TS3",&s3); } 
   //if(info->Tip())       { tree->Branch("TTip","TTip",&tip); } 

   //if(info->Griffin())   { tree->Branch("TGriffin","TGriffin",&griffin); } 
   //if(info->Sceptar())   { tree->Branch("TSceptar","TSceptar",&sceptar); } 
   //if(info->Paces())     { tree->Branch("TPaces","TPaces",&paces); } 
   //if(info->Dante())     { tree->Branch("TDante","TDante",&dante); } 
   //if(info->ZeroDegree()){ tree->Branch("TZeroDegree","TZeroDegree",&zerodegree); } 
   //if(info->Descant())   { tree->Branch("TDescant","TDescant",&descant);
   return;  
}




void TAnalysisTreeBuilder::CloseAnalysisFile() {
   if(!fCurrentAnalysisFile)
      return;

   fCurrentAnalysisFile->cd();
   if(fCurrentAnalysisTree)
      fCurrentAnalysisTree->Write();
   fCurrentAnalysisFile->Close();

   fCurrentAnalysisTree = 0;
   fCurrentAnalysisFile = 0;
   return;
}




