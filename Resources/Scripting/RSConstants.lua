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
  WordsBreakdown = 8067,
  SentencesBreakdown = 8074,
  ReadabilityScores = 8068,
  Grammar = 8069,
  Dolch = 8070,
  Warnings = 8045,
  BoxPlots = 8071,
  Histograms = 8072,
  Statistics = 8073
  }

ReportType =
  {
  StatisticsSummaryReport = 8055,
  ReadabilityScoresTabularReport = 8057,
  ReadabilityScoresSummaryReport = 8058
  }

GraphType =
  {
  WordBarChart = 8022,
  SentenceBoxPlox = 8035,
  SentenceHistogram = 8036,
  SentenceHeatmap = 8038,
  Fry = 8023,
  GpmFry = 8034,
  Raygor = 8024,
  Schwartz = 8026,
  Lix = 8027,
  DolchCoverageChart = 8032,
  DolchWordBarChart = 8033,
  Frase = 8025,
  Flesch = 8030,
  Crawford = 8031,
  SyllableHistogram = 8037,
  GermanLix = 8028,
  DanielsonBryan2 = 8029,
  WordCloud = 8039
  }

DolchResultType =
  {
  HighlightedDolchWordsReport = 8052,
  HighlightedNonDolchWordsReport = 8053,
  DolchWordsList = 8049,
  NonDolchWordsList = 8050,
  UnusedDolchWordsList = 8051,
  DolchCoverageChart = 8032,
  DolchBreakDownChart = 8033,
  DolchStatisticsSummary = 8054,
  DolchWordsBatchSummary = 8043,
  NonDolchWordsBatchSummary = 8044,
  DolchCoverageBatchSummary = 8042
  }

ListType =
  {
  ConjunctionStartingSentences = 8001,
  ArticleMismatch = 8007,
  PassiveVoice = 8008,
  Cliches = 8005,
  MisspelledWords = 8020,
  LowercasedSentences = 8002,
  WordingErrors = 8021,
  RedundantPhrases = 8004,
  Wordiness = 8003,
  ThreePlusSyllableWords = 8010,
  SixPlusCharacterWords = 8011,
  DaleChallUnfamiliarWords = 8012,
  SpacheUnfamiliarWords = 8013,
  HarrisJacobsonUnfamiliarWords = 8019,
  AllWords = 8014,
  AllWordsCondensed = 8015,
  OverusedWordsBySentence = 8018,
  ProperNouns = 8016,
  Contractions = 8017,
  ConjunctionStartingSentences = 8001,
  WordyItems = 8003,
  RedundantPhrases = 8004,
  RepeatedWords = 8006,
  LongSentences = 8009,
  MisspelledWords = 8020,
  WordingErrors = 8021,
  DolchWords = 8049,
  NonDolchWords = 8050,
  UnusedDolchWords = 8051,
  StatisticsTabularReport = 8056,
  BatchRawScores = 8040,
  BatchScoresSummary = 8046,
  BatchAggregatedGradeScoresSummary = 8047,
  BatchAggregatedClozeScoresSummary = 8048,
  BatchDifficultWords = 8041,
  BatchDolchCoverage = 8042,
  BatchDolchWords = 8043,
  BatchNonDolchWords = 8044,
  BatchWarning = 8045,
  Goals = 8060
  }

BatchScoreResultType =
  {
  RawScores = 8040,
  AggregatedScoreStatistics = 8046,
  AggregatedGradeScoresByDocument = 8047,
  AggregatedClozeScoresByDocument = 8048
  }

HighlightedReportType =
  {
  ThreePlusSyllableHighlightedWords = 8065,
  SixPlusCharacterHighlightedWords = 8061,
  DaleChallUnfamiliarHighlightedWords = 8062,
  SpacheUnfamiliarHighlightedWords = 8063,
  HarrisJacobsonUnfamiliarHighlightedWords = 8064,
  GrammarHighlightedIssues = 8066,
  DolchHighlightedWords = 8052,
  NonDolchHighlightedWords = 8053
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

BoxEffect =
  {
  Solid = 0,
  Glass = 1,
  FadeFromBottomToTop = 2,
  FadeFromTopToBottom = 3,
  StippleImage = 4,
  StippleShape = 5,
  Watercolor = 6,
  CommonImage = 7
  }

ImageEffect =
  {
  NoEffect = 0,
  Grayscale = 1,
  BlurHorizontal = 2,
  BlurVertical = 3,
  Sepia = 4,
  FrostedGlass = 5,
  OilPainting = 6
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
BoxEffect = protect_enum(BoxEffect)
ImageEffect = protect_enum(ImageEffect)
SortOrder = protect_enum(SortOrder)
ReportType = protect_enum(ReportType)
SideBarSection = protect_enum(SideBarSection)
GraphType = protect_enum(GraphType)
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
