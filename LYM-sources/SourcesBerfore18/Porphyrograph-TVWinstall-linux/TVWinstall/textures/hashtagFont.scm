(define (script-fu-text-box inText inFont inFontSize inTextColor)
  (let*
    (
      (fontSize 50)

      ;define some local variables
      (theImageWidth 600)
      (theLayerWidth 600)
      (theImageHeight (* fontSize 4))
      ;empty declaration for use later.
      (theText)
      (theImage)
      (theLayer)
      (theString1 "ABCDEFGHIJKLM") ; BCDEFGHIJKLMNOPQRSTUVWXYZabcdef#_
      (theString2 "NOPQRSTUVWXYZ") ; BCDEFGHIJKLMNOPQRSTUVWXYZabcdef#_
      (theString3 "abcdefghijklm") ; BCDEFGHIJKLMNOPQRSTUVWXYZabcdef#_
      (theString4 "nopqrstuvwxyz#_") ; BCDEFGHIJKLMNOPQRSTUVWXYZabcdef#_
      (theSubString)
      (leftMargin 10)
      (curX leftMargin)
      (curY 0)
      (curIndex 0)
      (p (open-output-file "/home/christian/LYM/TVWinstall/textures/hashtagOffsets.txt"))
    )
    (set!     ;create a new image:
     theImage
        (car 
         (gimp-image-new theImageWidth theImageHeight RGB)
        )
    )
    (set! 
     theLayer
        (car
         (gimp-layer-new theImage theImageWidth theImageHeight 
             RGBA-IMAGE "foobar" 100 NORMAL)
         )
    )
  
    (gimp-image-add-layer theImage theLayer 0)
    (gimp-context-set-background '(255 255 255))
    (gimp-context-set-foreground '(0 0 0))
    (gimp-drawable-fill theLayer BACKGROUND-FILL)

    (set! curY  0 )
    (set! curX  leftMargin )
    (set! curIndex  0 )

    ;(display theString1 p)(newline p)
    (while (< curIndex (string-length theString1))
      (set!     ;create a new image:
       theSubString
           (substring theString1 curIndex (+ curIndex 1))
      )
      (set! 
       theText
       (car 
        (gimp-text-fontname theImage theLayer curX curY 
          theSubString 0 TRUE fontSize PIXELS "Myriad Pro Bold")
       )
      )
      (set! theLayerWidth   (car (gimp-drawable-width  theText) ) )
      (gimp-floating-sel-anchor theText)

      (write curX p)(write-char #\space p)(write curY p)(write-char #\space p)(write theLayerWidth p)(write-char #\space p)(write fontSize p)(newline p)

      (set! curX  (+ curX theLayerWidth) )
      (set! curIndex (+ curIndex 1))
    )

    (set! curY  (+ curY fontSize) )
    (set! curX  leftMargin )
    (set! curIndex  0 )

    ;(display theString2 p)(newline p)
    (while (< curIndex (string-length theString2))
      (set!     ;create a new image:
       theSubString
           (substring theString2 curIndex (+ curIndex 1))
      )
      (set! 
       theText
       (car 
        (gimp-text-fontname theImage theLayer curX curY 
          theSubString 0 TRUE fontSize PIXELS "Myriad Pro Bold")
       )
      )
      (set! theLayerWidth   (car (gimp-drawable-width  theText) ) )
      (gimp-floating-sel-anchor theText)

      (write curX p)(write-char #\space p)(write curY p)(write-char #\space p)(write theLayerWidth p)(write-char #\space p)(write fontSize p)(newline p)

      (set! curX  (+ curX theLayerWidth) )
      (set! curIndex (+ curIndex 1))
    )
    
    (set! curY  (+ curY fontSize) )
    (set! curX  leftMargin )
    (set! curIndex  0 )

    ;(display theString3 p)(newline p)
    (while (< curIndex (string-length theString3))
      (set!     ;create a new image:
       theSubString
           (substring theString3 curIndex (+ curIndex 1))
      )
      (set! 
       theText
       (car 
        (gimp-text-fontname theImage theLayer curX curY 
          theSubString 0 TRUE fontSize PIXELS "Myriad Pro Bold")
       )
      )
      (set! theLayerWidth   (car (gimp-drawable-width  theText) ) )
      (gimp-floating-sel-anchor theText)

      (write curX p)(write-char #\space p)(write curY p)(write-char #\space p)(write theLayerWidth p)(write-char #\space p)(write fontSize p)(newline p)

      (set! curX  (+ curX theLayerWidth) )
      (set! curIndex (+ curIndex 1))
    )
    
    (set! curY  (+ curY fontSize) )
    (set! curX  leftMargin )
    (set! curIndex  0 )

    ;(display theString4 p)(newline p)
    (while (< curIndex (string-length theString4))
      (set!     ;create a new image:
       theSubString
           (substring theString4 curIndex (+ curIndex 1))
      )
      (set! 
       theText
       (car 
        (gimp-text-fontname theImage theLayer curX curY 
          theSubString 0 TRUE fontSize PIXELS "Myriad Pro Bold")
       )
      )
      (set! theLayerWidth   (car (gimp-drawable-width  theText) ) )
      (gimp-floating-sel-anchor theText)

      (write curX p)(write-char #\space p)(write curY p)(write-char #\space p)(write theLayerWidth p)(write-char #\space p)(write fontSize p)(newline p)

      (set! curX  (+ curX theLayerWidth) )
      (set! curIndex (+ curIndex 1))
    )

    (close-output-port p)

    ;(gimp-layer-resize theLayer theImageWidth theImageHeight 0 0)
    ;(gimp-image-resize theImage theImageWidth theImageHeight 0 0)
    ;(gimp-display-new theImage)
    (gimp-file-save RUN-NONINTERACTIVE theImage theLayer 
      ;(string-append (string-append "/home/christian/LYM/TVWinstall/textures/text_" theSubString) ".png")
      ;(string-append (string-append "/home/christian/LYM/TVWinstall/textures/text_" theSubString) ".png") )
      "/home/christian/LYM/TVWinstall/textures/hashtagFont.png"
      "/home/christian/LYM/TVWinstall/textures/hashtagFont.png" )
    (gimp-image-delete theImage)

  )
)
(script-fu-register
 "script-fu-text-box"                       ;func name*
 "Text Box 2"                                 ;menu label*
 "Created a simple text box, sized to fit\
    around the user's coice of text,\
    font, font size, and color."            ;description*
 "Stephen Huenneke"      ;Author*
 "copyright never"       ;Copyright*
 "April 30, 2011"        ;create date*
 ""                      ;image type this works on*
 SF-STRING        "TEXT"         "XYZ" ;a string variable
 SF-FONT          "FONT"         "Myriad Pro Bold"  ;a font variable
 SF-ADJUSTMENT    "FONT SIZE"    '(50 1 1000 1 10 0 1) ;a spin-button
 SF-COLOR         "COLOR"        '(0 0 0)   ;a color variable
)
(script-fu-menu-register "script-fu-text-box" "<Image>/Layer")