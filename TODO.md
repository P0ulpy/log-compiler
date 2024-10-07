# Log Compiler - Features Checklist

## v1.0.0 release

- [x] Core Systems
    - [x] Parsing
    - [x] Data layout
    - [x] Tokenization
- [ ] Generation
    - [x] Markdown
    - [x] Json
    - [ ] C + RayGui
- [ ] C + RayGui compilation to executable
- [x] CLI
    - [x] Input file
    - [x] Output file
    - [x] Format
    - [x] Verbose / Debug info
    - [x] Help / version
- [x] Gramar implementation
    - [x] InputFile
    - [x] Entry
    - [x] TitleSection
    - [x] TitleLevel
    - [x] Blockquote
    - [x] BlockquoteLine
    - [x] SpacingChar
    - [x] Line
    - [x] Text
    - [x] EmptyLine
    - [x] EndLine
- [ ] Code enhancement
    - [ ] Generalize visitor patern to Parsing and tokenization phase
- [ ] Testing
    - [ ] Input file
    - [ ] Parsing
    - [ ] Tokenization
    - [ ] Generation
- [ ] BugFix
    - [ ] ...

- [ ] Reflexion around evolutions for future versions
    - [ ] Extend grammar to handle more of mardown stylax
    - [ ] Allow multiple input files