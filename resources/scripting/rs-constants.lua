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

-- The following sorting directions are available.
SortOrder =
  {
  SortAscending = 0, -- Sort lowest to highest (i.e., A-Z, 1-9).
  SortDescending = 1 -- Sort highest to lowest (i.e., Z-A, 9-1).
  }

-- The following values are available for selection a section in a project.
SideBarSection =
  {
  WordsBreakdown = 8067,     -- The words breakdown section.
  SentencesBreakdown = 8074, -- The sentences breakdown section.
  ReadabilityScores = 8068,  -- The readability scores section.
  Grammar = 8069,            -- The grammar section.
  Dolch = 8070,              -- The **Dolch** section.
  Warnings = 8045,           -- The warnings section (batch projects only).
  BoxPlots = 8071,           -- The box plots section (batch projects only).
  Histograms = 8072,         -- The histograms section (batch projects only).
  Statistics = 8073          -- The summary statistics section.
  }

-- The following report types are available.
ReportType =
  {
  StatisticsSummaryReport = 8055,        -- The statistics summary report.
  ReadabilityScoresTabularReport = 8057, -- The statistics summary report (in tabular format).
  ReadabilityScoresSummaryReport = 8058  -- The readability-scores summary report.
  }

-- The following graph types are available.
GraphType =
  {
  WordBarChart = 8022,       -- Bar chart showing the totals of various word categories (standard projects only).
  SentenceBoxPlox = 8035,    -- Box plot showing the distribution of the document's sentence lengths (standard project only).
  SentenceHistogram = 8036,  -- Histogram showing the distribution of the document's sentence lengths (standard project only).
  SentenceHeatmap = 8038,    -- Heatmap showing the density of sentence lengths.
  Fry = 8023,                -- **Fry** readability graph.
  GpmFry = 8034,             -- **Gilliam-Peña-Mountain** readability graph.
  Raygor = 8024,             -- **Raygor Estimate** graph.
  Schwartz = 8026,           -- **Schwartz** graph.
  Lix = 8027,                -- **Lix** Gauge
  DolchCoverageChart = 8032, -- Bar chart showing the coverage of each **Dolch** word category.
  DolchWordBarChart = 8033,  -- Bar chart showing the total of each **Dolch** word category.
  Frase = 8025,              -- **FRASE** graph.
  Flesch = 8030,             -- **Flesch Reading Ease** chart.
  Crawford = 8031,           -- **Crawford** readability graph.
  SyllableHistogram = 8037,  -- Histogram showing the distribution of words by syllable size.
  SyllablePieChart = 8075,   -- Pie chart showing the distribution of words by syllable size.
  GermanLix = 8028,          -- German variation of Lix.
  DanielsonBryan2 = 8029,    -- **Danielson Bryan 2** readability graph (a variation of **Flesch Reading Ease**).
  WordCloud = 8039           -- Word cloud of key words.
  }

-- The following Dolch result windows are available.
DolchResultType =
  {
  HighlightedDolchWordsReport = 8052,    -- The highlighted **Dolch** word report (standard projects only).
  HighlightedNonDolchWordsReport = 8053, -- The highlighted **non-Dolch** word report (standard projects only).
  DolchWordsList = 8049,                 -- The list of used **Dolch** words (standard projects only).
  NonDolchWordsList = 8050,              -- The list of **non-Dolch** words (standard projects only).
  UnusedDolchWordsList = 8051,           -- The list of unused **Dolch** words (standard projects only).
  DolchCoverageChart = 8032,             -- Bar chart showing the coverage of each **Dolch** category (standard projects only).
  DolchBreakDownChart = 8033,            -- Bar chart showing the totals of each **Dolch** category (standard projects only).
  DolchStatisticsSummary = 8054,         -- The **Dolch** statistics summary report (standard projects only).
  DolchWordsBatchSummary = 8043,         -- The **Dolch** summary list (batch projects only).
  NonDolchWordsBatchSummary = 8044,      -- The **non-Dolch** summary list (batch projects only).
  DolchCoverageBatchSummary = 8042       -- The **Dolch** coverage list (batch projects only).
  }

-- The following types of lists are available.
ListType =
  {
  ConjunctionStartingSentences = 8001,      -- The list of conjunction-starting sentences.
  ArticleMismatch = 8007,                   -- The list of article mismatched.
  PassiveVoice = 8008,                      -- The list of passive voice phrases.
  Cliches = 8005,                           -- The clichés list.
  LowercasedSentences = 8002,               -- The lowercased sentences list.
  WordingErrors = 8021,                     -- The list of wording errors.
  Wordiness = 8003,                         -- The wordy items list.
  ThreePlusSyllableWords = 8010,            -- The 3+ syllable word list (standard projects only).
  SixPlusCharacterWords = 8011,             -- The 6+ character word list (standard projects only).
  DaleChallUnfamiliarWords = 8012,          -- The **Dale-Chall** unfamiliar word list (standard projects only).
  SpacheUnfamiliarWords = 8013,             -- The **Spache** unfamiliar word list (standard projects only).
  HarrisJacobsonUnfamiliarWords = 8019,     -- The **Harris-Jacobson** unfamiliar word list (standard projects only).
  AllWords = 8014,                          -- The list of all words from the document (standard projects only).
  AllWordsCondensed = 8015,                 -- The list of all important words, combined by their roots.
  OverusedWordsBySentence = 8018,           -- The overly used words (x sentences) list.
  RedundantPhrases = 8004,                  -- The redundant phrases list.
  RepeatedWords = 8006,                     -- The repeated words list.
  LongSentences = 8009,                     -- The overly-long sentences list.
  MisspelledWords = 8020,                   -- The misspelled-words list.
  DolchWords = 8049,                        -- The list of used **Dolch** words (standard projects only).
  NonDolchWords = 8050,                     -- The list of **non-Dolch** words (standard projects only).
  UnusedDolchWords = 8051,                  -- The list of unused **Dolch** words (standard projects only).
  StatisticsTabularReport = 8056,           -- The statistics summary report, in tabular format (standard projects only).
  BatchRawScores = 8040,                    -- The list of readability scores from a batch project.
  BatchScoresSummary = 8046,                -- The readability-scores aggregated statistics report (batch projects only).
  BatchAggregatedGradeScoresSummary = 8047, -- The grade-scores aggregated statistics report, listed by document (batch projects only).
  BatchAggregatedClozeScoresSummary = 8048, -- The Cloze-scores aggregated statistics report, listed by document (batch projects only).
  BatchDifficultWords = 8041,               -- The difficult words summary list (batch projects only).
  BatchDolchCoverage = 8042,                -- The **Dolch** coverage list (batch projects only).
  BatchDolchWords = 8043,                   -- The **Dolch** summary list (batch projects only).
  BatchNonDolchWords = 8044,                -- The **non-Dolch** summary list (batch projects only).
  BatchWarning = 8045,                      -- The warnings list (batch projects only).
  Goals = 8060                              -- The list of goals that either passed or failed in the project.
  }

-- The following highlighted batch project result windows are available.
BatchScoreResultType =
  {
  RawScores = 8040,                       -- The scores.
  AggregatedScoreStatistics = 8046,       -- The aggregated score statistics.
  AggregatedGradeScoresByDocument = 8047, -- The aggregated grade-level scores (x document).
  AggregatedClozeScoresByDocument = 8048  -- The aggregated cloze scores (x document).
  }

-- The following highlighted report windows are available.
HighlightedReportType =
  {
  ThreePlusSyllableHighlightedWords = 8065,        -- The highlighted 3+ syllable word report.
  SixPlusCharacterHighlightedWords = 8061,         -- The highlighted 6+ character word report.
  DaleChallUnfamiliarHighlightedWords = 8062,      -- The highlighted **Dale-Chall** unfamiliar word report.
  SpacheUnfamiliarHighlightedWords = 8063,         -- The highlighted **Spache** unfamiliar word report.
  HarrisJacobsonUnfamiliarHighlightedWords = 8064, -- The highlighted **Harris-Jacobson** unfamiliar word report.
  GrammarHighlightedIssues = 8066,                 -- The highlighted grammar issues word report.
  DolchHighlightedWords = 8052,                    -- The highlighted **Dolch** word report.
  NonDolchHighlightedWords = 8053                  -- The highlighted **non-Dolch** word report.
  }

-- The following test types are available.
TestType =
  {
  GradeLevel = 0,                       -- A test yielding a grade level.
  IndexValue = 1,                       -- A test yielding an index score.
  IndexValueAndGradeLevel = 2,          -- A test yielding a grade level and index score.
  PredictedClozeScore = 3,              -- A test yielding a predicted cloze score.
  GradeLevelAndPredictedClozeScore  = 4 -- A test yielding a grade level and predicted cloze score.
  }

-- The following text exclusion methods are available.
TextExclusionType =
  {
  ExcludeIncompleteSentences = 0,                 -- All incomplete sentences should be excluded from the analysis.
  DoNotExcludeAnyText = 1,                        -- No text should be excluded from analysis.
  ExcludeIncompleteSentencesExceptForHeadings = 2 -- All incomplete sentences (except headers and footers) should be excluded from the analysis.
  }

-- The following stemmers are available.
StemmingType =
  {
  NoStemming = 0, -- Do not stem any words.
  Danish = 1,     -- Danish
  Dutch = 2,      -- Dutch
  English = 3,    -- English
  Finnish = 4,    -- Finnish
  French = 5,     -- French
  German = 6,     -- German
  Italian = 7,    -- Italian
  Norwegian = 8,  -- Norwegian
  Portuguese = 9, -- Portuguese
  Spanish = 10,   -- Spanish
  Swedish = 11    -- Swedish
  }

-- The following releases of *Readability Studio* are available.
ProductType =
  {
  ProductWindows32Bit = 0,      -- Windows 32-bit
  ProductMac32BitPowerPC = 1,   -- OSX 32-bit PowerPC (discontinued after 2013)
  ProductMac32BitIntel = 2,     -- OSX 32-bit Intel (discontinued after 2015)
  ProductMac64BitIntel = 3      -- macOS 64-bit Intel
  }

-- The following bar chart and box plot effects are available.
BoxEffect =
  {
  Solid = 0,               -- A solid color.
  Glass = 1,               -- A "glassy" look.
  FadeFromBottomToTop = 2, -- A color fade across the bars (starting from the bottom).
  FadeFromTopToBottom = 3, -- A color fade across the bars (starting from the top).
  StippleImage = 4,        -- Fill with a repeating image.
  StippleShape = 5,        -- Fill with a repeating shape.
  Watercolor = 6,          -- A watercolor-like effect, where the box is warped and looks like it was filled in with watercolor paint (or a marker).
  ThickWatercolor = 7,     -- Same as WaterColor, but applies a second coat of paint, making it appear thicker.
  CommonImage = 8          -- A subimage of a larger image shared by all boxes.
  }

-- Effects that could be applied to an image.
ImageEffect =
  {
  NoEffect = 0,       -- Do no alter the image.
  Grayscale = 1,      -- Shades of gray (i.e., Black & White).
  BlurHorizontal = 2, -- A horizontal blur across the image.
  BlurVertical = 3,   -- A vertical blur across the image.
  Sepia = 4,          -- A sepia (i.e., faded photograph) effect.
  FrostedGlass = 5,   -- A frosted glass window effect to an image.
  OilPainting = 6     -- An oil painting effect.
  }

-- How to fit an image into of a rectangle.
ImageFit =
  {
  CropAndCenter = 0, -- Crop the image to the dimensions of the rect, with the center of the image as the crop origin.
  Shrink = 1         -- Shrink the image to the rect.
  }

-- The following types of orientation are available.
Orientation =
  {
  Horizontal = 0, -- Left and right.
  Vertical = 1    -- Up and down.
  }

-- The following tests are available.
Test =
  {
  Ari = "ari-test",                                                       -- Automated Readability Index
  ColemanLiau = "coleman-liau-test",                                      -- Coleman-Liau
  DaleChall = "dale-chall-test",                                          -- New Dale-Chall
  Forcast = "forcast-test",                                               -- FORCAST
  Flesch = "flesch-test",                                                 -- Flesch Reading Ease
  FleschKincaid = "flesch-kincaid-test",                                  -- Flesch-Kincaid
  Fry = "fry-test",                                                       -- Fry Graph
  GunningFog = "gunning-fog-test",                                        -- Gunning Fog
  Lix = "lix-test",                                                       -- Lix
  Eflaw = "eflaw-test",                                                   -- Eflaw
  NewFog = "new-fog-count-test",                                          -- New Fog Count
  Raygor = "raygor-test",                                                 -- Raygor Estimate
  Rix = "rix-test",                                                       -- RIX
  SimpleAri = "new-ari-simplified",                                       -- New Automated Readability Index (Kincaid, simplified)
  Spache = "spache-test",                                                 -- Spache
  Smog = "smog-test",                                                     -- SMOG
  SmogSimplified = "smog-test-simplified",                                -- SMOG (Simplified)
  ModifiedSmog = "modified-smog",                                         -- Modified SMOG
  PskFlesch = "psk-test",                                                 -- Powers, Sumner, Kearl (Flesch)
  HarrisJacobson = "harris-jacobson",                                     -- Harris-Jacobson
  PskDaleChall = "psk-dale-chall",                                        -- Powers, Sumner, Kearl (Dale-Chall)
  BormuthClozeMmean = "bormuth-cloze-mean-machine-passage",               -- Bormuth Cloze Mean
  BormuthGradePlacement35 = "bormuth-grade-placement-35-machine-passage", -- Bormuth Grade Placement
  PskGunningFog = "psk-fog",                                              -- Powers, Sumner, Kearl (Gunning Fog)
  FarrJenkinsPaterson = "farr-jenkins-paterson",                          -- Farr, Jenkins, Paterson
  NewFarrJenkinsPaterson = "new-farr-jenkins-paterson",                   -- New Farr, Jenkins, Paterson
  PskFarrJenkinsPaterson = "psk-farr-jenkins-paterson",                   -- Powers, Sumner, Kearl (Farr, Jenkins, Paterson)
  WheelerSmith = "wheeler-smith",                                         -- Wheeler-Smith
  GpmFry = "gilliam-pena-mountain-fry-graph",                             -- Gilliam-Peña-Mountain Graph
  Frase = "frase",                                                        -- FRASE
  Schwartz = "schwartz",                                                  -- Schwartz Graph (German)
  Crawford = "crawford",                                                  -- Crawford
  SolSpanish = "sol-spanish",                                             -- SOL (Spanish)
  DegreesOfReadingPower = "degrees-of-reading-power",                     -- Degrees of Reading Power
  DegreesOfReadingPowerGe = "degrees-of-reading-power-grade-equivalent",  -- Degrees of Reading Power (GE)
  NewAri = "new-ari",                                                     -- New Automated Readability Index
  FleschKincaidSimplified = "flesch-kincaid-test-simplified",             -- Flesch-Kincaid (simplified)
  Dolch = "dolch",                                                        -- Dolch
  Amstad = "amstad",                                                      -- Amstad
  SmogBambergerVanecek = "smog-bamberger-vanecek",                        -- SMOG (Bamberger-Vanecek)
  WheelerSmithBambergerVanecek = "wheeler-smith-bamberger-vanecek",       -- Wheeler-Smith (Bamberger-Vanecek)
  Qu = "qu-bamberger-vanecek",                                            -- Qu (Quadratwurzelverfahren)
  NeueWienerSachtextformel1 = "neue-wiener-sachtextformel1",              -- Neue Wiener Sachtextformel (1)
  NeueWienerSachtextformel2 = "neue-wiener-sachtextformel2",              -- Neue Wiener Sachtextformel (2)
  NeueWienerSachtextformel3 = "neue-wiener-sachtextformel3",              -- Neue Wiener Sachtextformel (3)
  LixGermanChildrensLiterature = "lix-german-childrens-literature",       -- Lix (German children's literature)
  LixGermanTechnical = "lix-german-technical",                            -- Lix (German technical literature)
  RixGermanFiction = "rix-german-fiction",                                -- Rix (German fiction)
  RixGermanNonfiction = "rix-german-nonfiction",                          -- Rix (German non-fiction)
  Elf = "easy-listening-formula",                                         -- Easy Listening Formula
  DanielsonBryan1 = "danielson-bryan-1",                                  -- Danielson-Bryan 1
  DanielsonBryan2 = "danielson-bryan-2"                                   -- Danielson-Bryan 2
  }

-- The following languages are available for projects.
Language =
  {
  English = 0, -- English.
  Spanish = 1, -- Spanish.
  German = 2   -- German.
  }

-- The following document connection methods are available.
TextStorage =
  {
  EmbedText = 0,  -- Embeds the document's text into the project.
  NoEmbedText = 1 -- Links the project to the external document.
  }

-- How paragraphs should be deduced.
ParagraphParse =
  {
  OnlySentenceTerminatedNewLinesAreParagraphs = 0, -- Hard returns only mark a new paragraph if they end with a terminated sentence.
  EachNewLineIsAParagraph = 1                      -- Each hard return marks the start of a new paragraph.
  }

-- The following values are available for controlling how filepaths in batch project lists are displayed.
FilePathDisplayMode =
  {
  TruncatePaths = 0,     -- Show the filename after an abbreviated filepath.
  OnlyShowFileNames = 1, -- Only show the filename.
  NoTruncation = 2       -- Show the full filepath.
  }

-- Weights that can be applied when setting a font.
FontWeight =
  {
  Thin = 100,        -- Thin.
  ExtraLight = 200,  -- Extra light.
  Light = 300,       -- Light.
  Normal = 400,      -- Normal.
  Medium = 500,      -- Medium.
  SemiBold = 600,    -- Semibold.
  Bold = 700,        -- Bold.
  ExtraBold = 800,   -- Extra bold.
  Heavy = 900,       -- Heavy.
  ExtraHeavy = 1000  -- Extra heavy.
  }

RibbonButtonBar =
  {
  Edit = 30001,
  Proofing = 30002,
  ParagraphDeduction = 30003,
  TextExclusion = 30004,
  Numerals = 30005,
  View = 30006,
  Clipboard = 30007,
  Document = 30008,
  Project = 30009
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
ImageFit = protect_enum(ImageFit)
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
Test = protect_enum(Test)
Language = protect_enum(Language)
TextStorage = protect_enum(TextStorage)
ParagraphParse = protect_enum(ParagraphParse)
FilePathDisplayMode = protect_enum(FilePathDisplayMode)
RibbonButtonBar = protect_enum(RibbonButtonBar)
FontWeight = protect_enum(FontWeight)
