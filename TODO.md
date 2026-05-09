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
- [ ] Gramar enhancement
    - [ ] make `\` beeing an escaping character insteed of a hard coded behaviour
    - [ ] Support links 
- [ ] Code enhancement
    - [ ] Generalize visitor patern to Parsing and tokenization phase
- [ ] Testing
    - [ ] Implement a simple testing system similar to tsoding/b
    - [ ] Input file
    - [ ] Parsing
    - [ ] Tokenization
    - [ ] Generation
- [ ] BugFix
    - [ ] ...

- [ ] Reflexion around evolutions for future versions
    - [ ] Extend grammar to handle more of mardown stylax
    - [ ] Allow multiple input files