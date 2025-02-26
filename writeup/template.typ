#let conf(doc, title: "Class Title", author: "") = {
  set page(paper: "a4")
  // set page(
  //   width: 3in,
  //   height: 6in,
  // )
  set par(justify: true)
  set text(
    font: "Times New Roman",
    size: 13pt,
  )


  // Heading show rules.

  align(horizon + center)[
    #v(2fr)
    #text(size: 50pt, weight: "bold", title)
    #v(1fr)
    #author
    #v(2fr)
  ]

  set heading(numbering: "1.")

  pagebreak()

  outline()

  pagebreak()

  doc
}
