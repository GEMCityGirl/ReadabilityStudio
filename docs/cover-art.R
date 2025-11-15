
# Make an image look like a Polaroid
creatPolaroid <- function(image,
                          caption = "Polaroid",
                          rotate = 3,
                          font_candidates = NULL)
  {
  # Default handwritten candidates
  if (is.null(font_candidates)) {
    font_candidates <- c(
      "Ink Free",
      "Gabriola",
      "Segoe Script",
      "Lucida Handwriting",
      "Bradley Hand",
      "Chalkboard",
      "Snell Roundhand"
    )
  }

  matchedFont <- font_candidates[font_candidates %in% magick_fonts()$family]

  captionFont <- "sans"
  if (length(matchedFont) > 0) {
    captionFont <- matchedFont[1]
    message(str_glue("✔ Using font family {captionFont} for Polariod effect."))
  } else {
    message("⚠ No magick-compatible handwritten font found; falling back to 'sans' for Polariod effect.")
  }

  # if a path, load it as an image
  if (!inherits(image, "magick-image")) image <- image_read(image)
  info <- image_info(image)
  w <- info$width
  h <- info$height

  # borders (proportional)
  sideBorder  <- round(w * 0.03)
  topBorder   <- round(h * 0.02)
  bottomExtra <- round(h * 0.15)

  # caption sizing/offsets (proportional)
  fontSize <- round(w * 0.05)           # 5% of width
  xOffset  <- round(w * 0.10)           # offset from left inside white border
  yOffset  <- round(bottomExtra * 0.6)  # lifted into bottom border

  # render the Polaroid
  image %>%
    image_border("white", geometry = str_glue("{sideBorder}x{topBorder}")) %>%
    image_extent(
      geometry = str_glue("{w + 2*sideBorder}x{h + topBorder + sideBorder + bottomExtra}"),
      gravity = "north",
      color = "white"
    ) %>%
    image_border("gray40", "2x2") %>%
    image_shadow() %>%
    # add a caption
    image_annotate(
      text     = caption,
      font     = captionFont,
      size     = fontSize,
      color    = "black",
      gravity  = "southwest",
      location = str_glue("+{xOffset}+{yOffset}")
    ) %>%
    # rotate
    image_rotate(rotate)
  }

# Creates a collage from a set of images for a manual cover.
# If captions contains three values, then the images will be displays as Polaroids
# with those captions.
createCover <- function(image1, image2, image3, outImage, captions = c())
  {
  imageLeft <- image_read(image1) %>%
    image_scale(geometry_area(1200))

  if (length(captions) >= 3)
    {
    imageLeft %<>% creatPolaroid(captions[1], 3)
    }

  imageRight <- image_read(image2) %>%
    image_scale(geometry_area(1000))

  if (length(captions) >= 3)
    {
    imageRight %<>% creatPolaroid(captions[2], 5)
    }

  imageBottom <- image_read(image3) %>%
    image_scale(geometry_area(1000))

  if (length(captions) >= 3)
    {
    imageBottom %<>% creatPolaroid(captions[3], -5)
    }

  manualCover <- magick::image_blank(width = 1913, height = 2200) %>%
    image_composite(imageRight, offset = "+750+500", operator = "DstOver") %>%
    image_composite(imageLeft %>% image_rotate(3), offset = "+50+325", operator = "DstOver") %>%
    image_composite(imageBottom, offset = "+250+1100", operator = "DstOver") %>%
    image_convert(depth = 8, format = "png")
  
  magick::image_write(manualCover, path = outImage)
  }