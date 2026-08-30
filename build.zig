const std = @import("std");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    var utf8proc = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    const lib = b.addLibrary(.{
        .name = "utf8proc",
        .linkage = .static,
        .root_module = utf8proc,
    });

    lib.installHeader(b.path("utf8proc.h"), "utf8proc.h");

    utf8proc.addCSourceFiles(.{ .root = b.path("."), .files = &.{
        "utf8proc.c",
    }, .flags = &.{"-DUTF8PROC_STATIC"} });

    b.installArtifact(lib);
}
