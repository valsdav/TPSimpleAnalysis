
import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('analysis')
options.parseArguments()

process = cms.Process("TPAnalysis")

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_Prompt_v12', '')


process.load("EventFilter.EcalRawToDigi.EcalUnpackerMapping_cfi")
process.load("EventFilter.EcalRawToDigi.EcalUnpackerData_cfi")

process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.source = cms.Source("NewEventStreamFileReader",
     fileNames = cms.untracked.vstring(options.inputFiles)
     )
     
process.tpAnalyzer = cms.EDAnalyzer("TPSimpleAnalysis",

    TPcollection = cms.InputTag("ecalEBunpacker","EcalTriggerPrimitives"),
    DigiCollectionEB = cms.InputTag("ecalEBunpacker","ebDigis"),
    DigiCollectionEE = cms.InputTag("ecalEBunpacker","eeDigis"),
    Print = cms.bool(True),

)

process.ecalDataSequence = cms.Sequence(process.ecalEBunpacker+process.tpAnalyzer)

process.p = cms.Path(process.ecalDataSequence)

#process.ecalEBunpacker.silentMode =  True 
process.ecalEBunpacker.silentMode = False 

process.output = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string(''),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string(options.outputFile),
    outputCommands = cms.untracked.vstring("keep *"),
    splitLevel = cms.untracked.int32(0)
)


process.endjob_step = cms.EndPath(process.output)


