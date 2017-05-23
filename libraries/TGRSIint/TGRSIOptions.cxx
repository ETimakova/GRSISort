#include "TGRSIOptions.h"

#include <algorithm>
#include <cctype>
#include <iostream>

#include "TEnv.h"

#include "Globals.h"
#include "ArgParser.h"
#include "DynamicLibrary.h"
#include "TGRSIUtilities.h"
#include "GRootCommands.h"

TGRSIOptions* TGRSIOptions::Get(int argc, char** argv)
{
   /// Gets the global instance of TGRSIOptions
   static TGRSIOptions* item = nullptr;
   if(!item) {
      item = new TGRSIOptions(argc, argv);
   }
   return item;
}

TGRSIOptions::TGRSIOptions(int argc, char** argv) : fShouldExit(false)
{
   /// Ctor used when interpreter is initialized
   Load(argc, argv);
}

void TGRSIOptions::Clear(Option_t*)
{
   /// Clears all of the variables in the TGRSIOptions
   fInputMidasFiles.clear();
   fInputLstFiles.clear();
   fInputRootFiles.clear();
   fInputCalFiles.clear();
   fInputOdbFiles.clear();
   fExternalRunInfo.clear();
   fMacroFiles.clear();

   fInputCutsFiles.clear();
   fInputValFiles.clear();
   fInputWinFiles.clear();
   fInputRing = "";

   fOutputFragmentFile          = "";
   fOutputAnalysisFile          = "";
   fOutputFilteredFile          = "";
   fOutputFragmentHistogramFile = "";
   fOutputAnalysisHistogramFile = "";

   fFragmentHistogramLib = "";
   fAnalysisHistogramLib = "";
   fCompiledFilterFile   = "";

   fOptionsFile.clear();

   fLogFile = "grsisort.log";

   fCloseAfterSort = false;
   fLogErrors      = false;
   fUseMidFileOdb  = false;

   fMakeAnalysisTree = false;
   fProgressDialog   = false;
   fReadingMaterial  = false;
   fIgnoreFileOdb    = false;

   fIgnoreScaler     = false;
   fIgnoreEpics      = false;
   fWriteBadFrags    = false;
   fWriteDiagnostics = false;

   fBatch = false;

   fShowedVersion = false;
   fHelp          = false;
   fShowLogo      = false;
   fSortRaw       = true;
   fSortRoot      = false;
   fExtractWaves  = false;
   fIsOnline      = false;
   fStartGui      = false;
   fMakeHistos    = false;
   fSortMultiple  = false;
   fDebug         = false;

   fFragmentWriteQueueSize = 10000000;
   fAnalysisWriteQueueSize = 1000000;

   fTimeSortInput = false;

   fBuildWindow        = 200;
   fAddbackWindow      = 300;
   fStaticWindow       = false;
   fSeparateOutOfOrder = false;

   fShouldExit = false;

   fColumnWidth         = 20;
   fStatusWidth         = 80;
   fStatusInterval      = 10;
   fLongFileDescription = false;

   // Proof only
   fMaxWorkers   = -1;
   fSelectorOnly = false;
}

void TGRSIOptions::Print(Option_t*) const
{
   /// Print the current status of TGRSIOptions, includes all names, lists and flags
   std::cout << "fCloseAfterSort: " << fCloseAfterSort << std::endl
             << "fLogErrors: " << fLogErrors << std::endl
             << "fUseMidFileOdb: " << fUseMidFileOdb << std::endl
             << "fSuppressErrors: " << fSuppressErrors << std::endl
             << "fReconstructTimeStamp: " << fReconstructTimeStamp << std::endl
             << std::endl
             << "fMakeAnalysisTree: " << fMakeAnalysisTree << std::endl
             << "fProgressDialog: " << fProgressDialog << std::endl
             << "fReadingMaterial;: " << fReadingMaterial << std::endl
             << "fIgnoreFileOdb: " << fIgnoreFileOdb << std::endl
             << "fRecordDialog: " << fRecordDialog << std::endl
             << std::endl
             << "fIgnoreScaler: " << fIgnoreScaler << std::endl
             << "fIgnoreEpics: " << fIgnoreEpics << std::endl
             << "fWriteBadFrags: " << fWriteBadFrags << std::endl
             << "fWriteDiagnostics: " << fWriteDiagnostics << std::endl
             << std::endl
             << "fBatch: " << fBatch << std::endl
             << std::endl
             << "fShowedVersion: " << fShowedVersion << std::endl
             << "fHelp: " << fHelp << std::endl
             << "fShowLogo: " << fShowLogo << std::endl
             << "fSortRaw: " << fSortRaw << std::endl
             << "fSortRoot: " << fSortRoot << std::endl
             << "fExtractWaves;: " << fExtractWaves << std::endl
             << "fIsOnline: " << fIsOnline << std::endl
             << "fStartGui: " << fStartGui << std::endl
             << "fMakeHistos: " << fMakeHistos << std::endl
             << "fSortMultiple: " << fSortMultiple << std::endl
             << "fDebug: " << fDebug << std::endl
             << "fLogFile: " << fLogFile << std::endl
             << std::endl
             << "fFragmentWriteQueueSize: " << fFragmentWriteQueueSize << std::endl
             << "fAnalysisWriteQueueSize: " << fAnalysisWriteQueueSize << std::endl
             << std::endl
             << "fTimeSortInput: " << fTimeSortInput << std::endl
             << "fSortDepth: " << fSortDepth << std::endl
             << std::endl
             << "fBuildWindow: " << fBuildWindow << std::endl
             << "fAddbackWindow: " << fAddbackWindow << std::endl
             << "fStaticWindow: " << fStaticWindow << std::endl
             << "fSeparateOutOfOrder: " << fSeparateOutOfOrder << std::endl
             << std::endl
             << "fShouldExit: " << fShouldExit << std::endl
             << std::endl
             << "fColumnWidth: " << fColumnWidth << std::endl
             << "fStatusWidth: " << fStatusWidth << std::endl
             << "fStatusInterval: " << fStatusInterval << std::endl
             << "fLongFileDescription: " << fLongFileDescription << std::endl

             << "fMaxWorkers: " << fMaxWorkers << std::endl
             << "fSelectorOnly " << fSelectorOnly << std::endl;
}

void TGRSIOptions::PrintSortingOptions() const
{
   /// Print just the sorting options in TGRSIOptions. This includes sort depth, build window, etc.
   std::cout << DBLUE << "fTimeSortInput: " << DCYAN << fTimeSortInput << std::endl
             << DBLUE << "fSortDepth:     " << DCYAN << fSortDepth << std::endl
             << DBLUE << "fBuildWindow:   " << DCYAN << fBuildWindow << std::endl
             << DBLUE << "fAddbackWindow: " << DCYAN << fAddbackWindow << std::endl
             << DBLUE << "fStaticWindow:  " << DCYAN << fStaticWindow << std::endl
             << DBLUE << "fSeparateOutOfOrder:  " << DCYAN << fSeparateOutOfOrder << std::endl
             << RESET_COLOR << std::endl;
}

void TGRSIOptions::Load(int argc, char** argv)
{
   /// This checks all of the options provided to GRSISort. This also loads in some
   /// libraries in order to do on the fly histogramming.
   Clear();
   fFragmentHistogramLib = gEnv->GetValue("GRSI.FragmentHistLib", "");
   fAnalysisHistogramLib = gEnv->GetValue("GRSI.AnalysisHistLib", "");

   // Load default TChannels, if specified.
   {
      std::string default_calfile = gEnv->GetValue("GRSI.CalFile", "");
      if(default_calfile.length()) {
         fInputCalFiles.push_back(default_calfile);
      }
   }

   // Load default GValues, if specified.
   {
      std::string default_valfile = gEnv->GetValue("GRSI.ValFile", "");
      if(default_valfile.length()) {
         fInputValFiles.push_back(default_valfile);
      }
   }

   ArgParser parser;

   std::vector<std::string> input_files;
   std::string              default_file_format;

   // parser.option() will initialize boolean values to false.

   parser.default_option(&input_files).description("Input file(s)");
   parser.option("output-fragment-tree", &fOutputFragmentFile).description("Filename of output fragment tree");
   parser.option("output-analysis-tree", &fOutputAnalysisFile).description("Filename of output analysis tree");
   parser.option("output-fragment-hists", &fOutputFragmentHistogramFile)
      .description("Filename of output fragment hists");
   parser.option("output-analysis-hists", &fOutputAnalysisHistogramFile)
      .description("Filename of output analysis hists");

   parser.option("a", &fMakeAnalysisTree).description("Make the analysis tree");
   parser.option("H histos", &fMakeHistos).description("attempt to run events through MakeHisto lib.");
   parser.option("g start-gui", &fStartGui).description("Start the gui at program start");
   parser.option("b batch", &fBatch).description("Run in batch mode");

   parser.option("sort-depth", &fSortDepth)
      .description("Number of events to hold when sorting by time/trigger_id")
      .default_value(200000);
   parser.option("s sort", &fSortRoot).description("Attempt to loop through root files.");

   parser.option("q quit", &fCloseAfterSort).description("Run in batch mode");
   parser.option("l no-logo", &fShowLogo).description("Inhibit the startup logo").default_value(true);
   parser.option("h help ?", &fHelp).description("Show this help message");
   parser.option("w extract-waves", &fExtractWaves)
      .description("Extract wave forms to data class when available.")
      .default_value(false);
   parser.option("d debug", &fDebug)
      .description("Write debug information to output/file, e.g. enables writing of TDescantDebug at analysis stage.")
      .default_value(false);
   parser.option("no-record-dialog", &fRecordDialog).description("Dump stuff to screen");
   parser.option("write-diagnostics", &fWriteDiagnostics);
   parser.option("log-errors", &fLogErrors);
   parser.option("log-file", &fLogFile).description("File logs from grsiproof are written to.");
   parser.option("reading-material", &fReadingMaterial);
   parser.option("bad-frags write-bad-frags bad-fragments write-bad-fragments", &fWriteBadFrags);
   parser.option("separate-out-of-order", &fSeparateOutOfOrder)
      .description("Write out-of-order fragments to a separate tree at the sorting stage")
      .default_value(false);
   parser.option("ignore-odb", &fIgnoreFileOdb);
   parser.option("ignore-epics", &fIgnoreEpics);
   parser.option("ignore-scaler", &fIgnoreScaler);
   parser.option("suppress-error suppress-errors suppress_error suppress_errors", &fSuppressErrors);
   parser.option("reconstruct-timestamp reconstruct-time-stamp", &fReconstructTimeStamp);

   parser.option("fragment-size", &fFragmentWriteQueueSize)
      .description("size of fragment write queue")
      .default_value(10000000);
   parser.option("analysis-size", &fAnalysisWriteQueueSize)
      .description("size of analysis write queue")
      .default_value(1000000);

   // parser.option("o output", &output_file)
   //   .description("Root output file");
   // parser.option("f filter-output",&fOutputFilteredFile)
   //   .description("Output file for raw filtered data");
   // parser.option("hist-output",&fOutputHistogramFile)
   //   .description("Output file for histograms");
   // parser.option("r ring",&fInputRing)
   //   .description("Input ring source (host/ringname).  Requires --format to be specified.");
   //   .default_value(false);
   // parser.option("n no-sort", &fSortRaw)
   //   .description("Load raw data files without sorting")
   //   .default_value(true);
   // parser.option("m sort-multiple", &fSortMultiple)
   //   .description("If passed multiple raw data files, treat them as one file.")
   //   .default_value(false);
   //   .default_value(false);
   // parser.option("t time-sort", &fTimeSortInput)
   //   .description("Reorder raw events by time");
   // parser.option("time-sort-depth",&fTimeSortDepth)
   //   .description("Number of events to hold when time sorting")
   //   .default_value(100000);
   parser.option("build-window", &fBuildWindow).description("Build window, timestamp units").default_value(200);
   parser.option("addback-window", &fAddbackWindow).description("Addback window, time in ns").default_value(300);
   parser.option("static-window", &fStaticWindow)
      .description("use static window for event building")
      .default_value(false);
   parser.option("column-width", &fColumnWidth).description("width of one column of status").default_value(20);
   parser.option("status-width", &fStatusWidth)
      .description("number of characters to be used for status output")
      .default_value(80);
   parser.option("status-interval", &fStatusInterval)
      .description(
         "seconds between each detailed status output (each a new line), non-positive numbers mean no detailed status")
      .default_value(10);

   // parser.option("long-file-description", &fLongFileDescription)
   //   .description("Show full path to file in status messages")
   //   .default_value(false);
   // parser.option("format",&default_file_format)
   //   .description("File format of raw data.  Allowed options are \"EVT\" and \"GEB\"."
   //                "If unspecified, will be guessed from the filename.");
   // parser.option("g start-gui",&fStartGui)
   //   .description("Start the GUI")
   //   .default_value(false);
   // parser.option("v version", &fShowedVersion)
   //   .description("Show version information");

   // Proof only parser options
   parser.option("max-workers", &fMaxWorkers)
      .description("Max number of nodes to use when running a grsiproof session")
      .default_value(-1);

   parser.option("selector-only", &fSelectorOnly).description("Turns off PROOF to run a selector on the main thread");

   // look for any arguments ending with .info, pass to parser.
   for(int i = 0; i < argc; i++) {
      std::string filename = argv[i];
      if(DetermineFileType(filename) == kFileType::CONFIG_FILE) {
         try {
            parser.parse_file(filename);
         } catch(ParseError& e) {
            std::cerr << "ERROR: " << e.what() << "\n" << parser << std::endl;
            fShouldExit = true;
         }
      }
   }

   // Look at the command line.
   try {
      parser.parse(argc, argv);
   } catch(ParseError& e) {
      std::cerr << "ERROR: " << e.what() << "\n" << parser << std::endl;
      fShouldExit = true;
   }

   // Print help if requested.
   if(fHelp) {
      Version();
      std::cout << parser << std::endl;
      fShouldExit = true;
   }

   // Print version if requested
   if(fShowedVersion) {
      Version();
      fShouldExit = true;
   }

   if(fOutputFragmentHistogramFile.length() > 0 && fOutputFragmentHistogramFile != "none") {
      fMakeHistos = true;
   }
   if(fOutputAnalysisHistogramFile.length() > 0 && fOutputAnalysisHistogramFile != "none") {
      fMakeHistos = true;
   }

   for(auto& file : input_files) {
      FileAutoDetect(file);
   }
}

kFileType TGRSIOptions::DetermineFileType(const std::string& filename) const
{
   size_t      dot_pos = filename.find_last_of('.');
   std::string ext     = filename.substr(dot_pos + 1);

   bool isZipped = (ext == "gz") || (ext == "bz2") || (ext == "zip");
   if(isZipped) {
      std::string remaining = filename.substr(0, dot_pos);
      ext                   = remaining.substr(remaining.find_last_of('.') + 1);
   }

   if(ext == "mid") {
      return kFileType::MIDAS_FILE;
   } else if(ext == "lst") {
      return kFileType::LST_FILE;
   } else if(ext == "evt") {
      return kFileType::NSCL_EVT;
   } else if(ext == "cal") {
      return kFileType::CALIBRATED;
   } else if(ext == "root") {
      return kFileType::ROOT_DATA;
   } else if((ext == "c") || (ext == "C") || (ext == "c+") || (ext == "C+") || (ext == "c++") || (ext == "C++")) {
      return kFileType::ROOT_MACRO;
   } else if(ext == "dat" || ext == "cvt") {
      if(filename.find("GlobalRaw") != std::string::npos) return kFileType::GRETINA_MODE3;
      return kFileType::GRETINA_MODE2;
   } else if(ext == "hist") {
      return kFileType::GUI_HIST_FILE;
   } else if(ext == "so") {
      return kFileType::COMPILED_SHARED_LIBRARY;
   } else if(ext == "info") {
      return kFileType::CONFIG_FILE;
   } else if(ext == "val") {
      return kFileType::GVALUE;
   } else if(ext == "win") {
      return kFileType::PRESETWINDOW;
   } else if(ext == "cuts") {
      return kFileType::CUTS_FILE;
   } else if(ext == "xml") {
      return kFileType::XML_FILE;
   } else {
      return kFileType::UNKNOWN_FILETYPE;
   }
}

bool TGRSIOptions::FileAutoDetect(const std::string& filename)
{
   /// Detects the type of file provided on the command line. This uses the extension to determine
   /// the type of the file. Once the type is determined, the file is sent to the appropriate list
   /// in TGRSIOptions. This is also smart enough to dynamically link histogramming libraries.
   switch(DetermineFileType(filename)) {
   case kFileType::NSCL_EVT:
   case kFileType::GRETINA_MODE2:
   case kFileType::GRETINA_MODE3:
   case kFileType::MIDAS_FILE: fInputMidasFiles.push_back(filename); return true;

   case kFileType::LST_FILE: fInputLstFiles.push_back(filename); return true;

   case kFileType::ROOT_DATA: fInputRootFiles.push_back(filename); return true;

   case kFileType::ROOT_MACRO: fMacroFiles.push_back(filename); return true;

   case kFileType::CALIBRATED: fInputCalFiles.push_back(filename); return true;

   case kFileType::COMPILED_SHARED_LIBRARY: {

      bool           used = false;
      DynamicLibrary lib(filename);
      if(lib.GetSymbol("MakeFragmentHistograms")) {
         fFragmentHistogramLib = filename;
         used                  = true;
      }
      if(lib.GetSymbol("MakeAnalysisHistograms")) {
         fAnalysisHistogramLib = filename;
         used                  = true;
      }
      if(!used) {
         std::cerr << filename << " did not contain MakeFragmentHistograms() or MakeAnalysisHistograms()" << std::endl;
      }
      return true;
   }

   case kFileType::GVALUE: fInputValFiles.push_back(filename); return true;

   case kFileType::PRESETWINDOW: fInputWinFiles.push_back(filename); return true;

   case kFileType::CUTS_FILE: fInputCutsFiles.push_back(filename); return true;

   case kFileType::CONFIG_FILE: return false;

   case kFileType::XML_FILE: fInputOdbFiles.push_back(filename); return true;

   case kFileType::UNKNOWN_FILETYPE:
   default: printf("\tDiscarding unknown file: %s\n", filename.c_str()); return false;
   }
}

std::string TGRSIOptions::GenerateOutputFilename(const std::string&)
{
   /// Currently does nothing
   return "temp.root";
}

std::string TGRSIOptions::GenerateOutputFilename(const std::vector<std::string>&)
{
   /// Currently does nothing
   return "temp_from_multi.root";
}
