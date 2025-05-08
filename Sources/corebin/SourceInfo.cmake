## The core binaries (cp, ls ...)
## this will also deliver custom ones, like "cmdreq" replacing "su", "sudo" and "doas"

CollectSources("${PROJECT_MODULE_ROOT}/cmdreq" CmdReq_Sources)

add_executable(cmdreq ${CmdReq_Sources})
target_link_options(cmdreq PRIVATE -static)