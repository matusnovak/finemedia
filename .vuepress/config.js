module.exports = {
    base: '/finemedia/',
    title: "FineMedia",
    description: "A simple image reading and writing library",
    themeConfig: {
        sidebar: 'auto',
        nav: [
            { text: 'Home', link: '/' },
            { text: 'Install', link: '/INSTALL' },
            { text: 'Examples', link: '/examples/' },
            { text: 'Modules', link: '/doxygen/modules' },
            {
                text: 'Classes',
                items: [
                    { text: 'Class List', link: '/doxygen/annotated' },
                    { text: 'Class Index', link: '/doxygen/classes' },
                    { text: 'Function Index', link: '/doxygen/functions' },
                    { text: 'Variable Index', link: '/doxygen/variables' },
                    { text: 'Enumeration Index', link: '/doxygen/enumerations' }
                ]
            },
            { text: 'Files', link: '/doxygen/files' },
            { text: 'Pages', link: '/doxygen/pages' }
        ]
    }
}
