target("unitest")
    set_kind("binary")
    set_default(true)
    add_deps("libco")
    add_files("*.cc")

