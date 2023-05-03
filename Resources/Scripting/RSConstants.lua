function protect_enum(tbl)
  return setmetatable ({}, 
    {
    __index = tbl,  -- read access gets original table item
    __newindex = function (t, n, v)
       error ("attempting to change constant " .. 
             tostring (n) .. " to " .. tostring (v), 2)
      end -- __newindex 
    })

end -- function protect_table

SortOrder =
  {
  SortAscending = 0,
  SortDescending = 1
  }

SideBarSection =
  {
  WordsBreakdown = 10053,
  SentencesBreakdown = 10079,
  ReadabilityScores = 10054,
  Grammar = 10057,
  Dolch = 10058,
  Warnings = 10059,
  BoxPlots = 10060,
  Histograms = 10061,
  Statistics = 10062
  }

ReportType =
  {
  StatisticsSummaryReport = 10042,
  ReadabilityScoresTabularReport = 10069,
  ReadabilityScoresSummaryReport = 10070
  }

GraphType =
  {
  WordBarChart = 10008,
  SentenceBoxPlox = 10063,
  SentenceHistogram = 10086,
  SentenceHeatmap = 10080,
  Fry = 10009,
  GpmFry = 10044,
  Raygor = 10010,
  Schwartz = 10082,
  Lix = 10083,
  DolchCoverageChart = 10012,
  DolchWordBarChart = 10013,
  Frase = 10047,
  Flesch = 10048,
  Crawford = 10049,
  SyllableHistogram = 10071,
  GermanLix = 10084,
  DanielsonBryan2 = 10085,
  ThreePlusSyllableWordCloud = 10088
  }

DolchResultType =
  {
  HighlightedDolchWordsReport = 10034,
  HighlightedNonDolchWordsReport = 10035,
  DolchWordsList = 10031,
  NonDolchWordsList = 10032,
  UnusedDolchWordsList = 10033,
  DolchCoverageChart = 10012,
  DolchBreakDownChart = 10013,
  DolchStatisticsSummary = 10037,
  DolchWordsBatchSummary = 10018,
  NonDolchWordsBatchSummary = 10019,
  DolchCoverageBatchSummary = 10017
  }

ListType =
  {
  ConjunctionStartingSentences = 10001,
  ArticleMismatch = 10072,
  PassiveVoice = 10073,
  Cliches = 10005,
  MisspelledWords = 10051,
  LowercasedSentences = 10002,
  WordingErrors = 10052,
  RedundantPhrases = 10004,
  Wordiness = 10003,
  ThreePlusSyllableWords = 10025,
  SixPlusCharacterWords = 10026,
  DaleChallUnfamiliarWords = 10027,
  SpacheUnfamiliarWords = 10028,
  HarrisJacobsonUnfamiliarWords = 10039,
  AllWords = 10029,
  AllWordsCondensed = 10078,
  OverusedWordsBySentence = 10077,
  ProperNouns = 10076,
  Contractions = 10081,
  ConjunctionStartingSentences = 10001,
  WordyItems = 10003,
  RedundantPhrases = 10004,
  RepeatedWords = 10006,
  LongSentences = 10007,
  MisspelledWords = 10051,
  WordingErrors = 10052,
  DolchWords = 10031,
  NonDolchWords = 10032,
  UnusedDolchWords = 10033,
  StatisticsTabularReport = 10043,
  BatchRawScores = 10015,
  BatchScoresSummary = 10021,
  BatchAggregatedGradeScoresSummary = 10045,
  BatchAggregatedClozeScoresSummary = 10046,
  BatchDifficultWords = 10055,
  BatchDolchCoverage = 10017,
  BatchDolchWords = 10018,
  BatchNonDolchWords = 10019,
  BatchWarning = 10059,
  Goals = 10087
  }

BatchScoreResultType =
  {
  RawScores = 10015,
  AggregatedScoreStatistics = 10021,
  AggregatedGradeScoresByDocument = 10045,
  AggregatedClozeScoresByDocument = 10046
  }

HighlightedReportType =
  {
  ThreePlusSyllableHighlightedWords = 10068,
  SixPlusCharacterHighlightedWords = 10064,
  DaleChallUnfamiliarHighlightedWords = 10065,
  SpacheUnfamiliarHighlightedWords = 10066,
  HarrisJacobsonUnfamiliarHighlightedWords = 10067,
  GrammarHighlightedIssues = 10036,
  DolchHighlightedWords = 10034,
  NonDolchHighlightedWords = 10035
  }

TestType =
  {
  GradeLevel = 0,
  IndexValue = 1,
  IndexValueAndGradeLevel = 2,
  PredictedClozeScore = 3,
  GradeLevelAndPredictedClozeScore  = 4
  }

TextExclusionType =
  {
  ExcludeIncompleteSentences = 0,
  DoNotExcludeAnyText = 1,
  ExcludeIncompleteSentencesExceptForHeadings = 2
  }

StemmingType =
  {
  NoStemming = 0,
  Danish = 1,
  Dutch = 2,
  English = 3,
  Finnish = 4,
  French = 5,
  German = 6,
  Italian = 7,
  Norwegian = 8,
  Portuguese = 9,
  Spanish = 10,
  Swedish = 11
  }

ProductType =
  {
  ProductWindows32BitIntel = 0,
  ProductMac32BitPowerPC = 1,
  ProductMac32BitIntel = 2,
  ProductMac64BitIntel = 3
  }

BarEffect =
  {
  Solid = 0,
  Glass = 1,
  FadeFromBottomToTop = 2,
  FadeFromTopToBottom = 3,
  CustomBrush = 4
  }

Orientation =
  {
  Horizontal = 0,
  Vertical = 1
  }

Tests =
  {
  Ari = "ari-test",
  ColemanLiau = "coleman-liau-test",
  DaleChall = "dale-chall-test",
  Forcast = "forcast-test",
  Flesch = "flesch-test",
  FleschKincaid = "flesch-kincaid-test",
  Fry = "fry-test",
  GunningFog = "gunning-fog-test",
  Lix = "lix-test",
  Eflaw = "eflaw-test",
  NewFog = "new-fog-count-test",
  Raygor = "raygor-test",
  Rix = "rix-test",
  SimpleAri = "new-ari-simplified",
  Spache = "spache-test",
  Smog = "smog-test",
  SmogSimplified = "smog-test-simplified",
  ModifiedSmog = "modified-smog",
  PskFlesch = "psk-test",
  HarrisJacobson = "harris-jacobson",
  PskDaleChall = "psk-dale-chall",
  BormuthClozeMmean = "bormuth-cloze-mean-machine-passage",
  BormuthGradePlacement35 = "bormuth-grade-placement-35-machine-passage",
  PskGunningFog = "psk-fog",
  FarrJenkinsPaterson = "farr-jenkins-paterson",
  NewFarrJenkinsPaterson = "new-farr-jenkins-paterson",
  PskFarrJenkinsPaterson = "psk-farr-jenkins-paterson",
  WheelerSmith = "wheeler-smith",
  GpmFry = "gilliam-pena-mountain-fry-graph",
  Frase = "frase",
  Schwartz = "schwartz",
  Crawford = "crawford",
  SolSpanish = "sol-spanish",
  DegreesOfReadingPower = "degrees-of-reading-power",
  DegreesOfReadingPowerGe = "degrees-of-reading-power-grade-equivalent",
  NewAri = "new-ari",
  FleschKincaidSimplified = "flesch-kincaid-test-simplified",
  Dolch = "dolch",
  Amstad = "amstad",
  SmogBambergerVanecek = "smog-bamberger-vanecek",
  WheelerSmithBambergerVanecek = "wheeler-smith-bamberger-vanecek",
  Qu = "qu-bamberger-vanecek",
  NeueWienerSachtextformel1 = "neue-wiener-sachtextformel1",
  NeueWienerSachtextformel2 = "neue-wiener-sachtextformel2",
  NeueWienerSachtextformel3 = "neue-wiener-sachtextformel3",
  LixGermanChildrensLiterature = "lix-german-childrens-literature",
  LixGermanTechnical = "lix-german-technical",
  RixGermanFiction = "rix-german-fiction",
  RixGermanNonfiction = "rix-german-nonfiction",
  Elf = "easy-listening-formula",
  DanielsonBryan1 = "danielson-bryan-1",
  DanielsonBryan2 = "danielson-bryan-2"
  }

Language =
  {
  English = 0,
  Spanish = 1,
  German = 2
  }

TextStorage =
  {
  EmbedText = 0,
  NoEmbedText = 1
  }

ParagraphParse =
  {
  OnlySentenceTerminatedNewLinesAreParagraphs = 0,
  EachNewLineIsAParagraph = 1
  }

FilePathDisplayMode =
  {
  TruncatePaths = 0,
  OnlyShowFileNames = 1,
  NoTruncation = 2
  }

-- Screenshot constants
-- (just used internally for taking documentation screenshots)
RibbonButtonBars =
  {
  Edit = 30001,
  Proofing = 30002,
  ParagraphDeduction = 30003,
  TextExclusion = 30004,
  Numerals = 30005
  }

OptionsPageType =
  {
  GeneralSettings = 1000,
  ProjectSettings = 2000,
  DocumentDisplayGeneral = 3000,
  DocumentDisplayDocumentView = 3001,
  DocumentDisplayDolch = 3002,
  ScoresTestOptions = 4000,
  ScoresDisplay = 4001,
  AnalysisDocumentIndexing = 5000,
  AnalysisDocumentStatistics  = 5001,
  GraphsGeneral = 6000,
  GraphsAxis = 6001,
  GraphsTitles = 6002,
  GraphsReadabilityGraphs = 6003,
  GraphsBarChart = 6004,
  GraphsHistograms = 6005,
  GraphsBoxPlot = 6006
  }

-- make the table into a constant enumeration
Orientation = protect_enum(Orientation)
BarEffect = protect_enum(BarEffect)
SortOrder = protect_enum(SortOrder)
ReportType = protect_enum(ReportType)
SideBarSection = protect_enum(SideBarSection)
GraphType = protect_enum(GraphType)
GrammarIssueType = protect_enum(GrammarIssueType)
HighlightedReportType = protect_enum(HighlightedReportType)
ListType = protect_enum(ListType)
DolchResultType = protect_enum(DolchResultType)
BatchScoreResultType = protect_enum(BatchScoreResultType)
TestType = protect_enum(TestType)
StemmingType = protect_enum(StemmingType)
ProductType = protect_enum(ProductType)
TextExclusionType = protect_enum(TextExclusionType)
OptionsPageType = protect_enum(OptionsPageType)
ProductType = protect_enum(ProductType)
Tests = protect_enum(Tests)
Language = protect_enum(Language)
TextStorage = protect_enum(TextStorage)
ParagraphParse = protect_enum(ParagraphParse)
FilePathDisplayMode = protect_enum(FilePathDisplayMode)
RibbonButtonBars = protect_enum(RibbonButtonBars)