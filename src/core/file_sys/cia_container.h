// Copyright 2017 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <string>
#include <vector>
#include "common/common_types.h"
#include "common/swap.h"
#include "core/file_sys/file_backend.h"
#include "core/file_sys/title_metadata.h"
#include "core/hle/service/fs/archive.h"
#include "core/loader/loader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// FileSys namespace

namespace FileSys {

static constexpr size_t CIA_CONTENT_INDEX_COUNT = 0x2000;
static constexpr size_t CIA_HEADER_SIZE = 0x2020;
static constexpr size_t CIA_DEPENDENCY_SIZE = 0x300;
static constexpr size_t CIA_METADATA_SIZE = 0x400;

/**
 * Helper which implements an interface to read and write CTR Installable Archive (CIA) files.
 * Data can either be loaded from a FileBackend, a string path, or from a data array. Data can
 * also be partially loaded for CIAs which are downloading/streamed in and need some metadata
 * read out.
 */
class CIAContainer {
public:
    // Load whole CIAs outright
    Loader::ResultStatus Load(FileBackend& backend);
    Loader::ResultStatus Load(std::string& filepath);
    Loader::ResultStatus Load(std::vector<u8>& header_data);

    // Load parts of CIAs (for CIAs streamed in)
    Loader::ResultStatus LoadHeader(std::vector<u8>& header_data, size_t offset = 0);
    Loader::ResultStatus LoadTitleMetadata(std::vector<u8>& tmd_data, size_t offset = 0);
    Loader::ResultStatus LoadMetadata(std::vector<u8>& meta_data, size_t offset = 0);

    TitleMetadata& GetTitleMetadata();
    std::array<u64, 0x30>& GetDependencies();
    u32 GetCoreVersion() const;

    u64 GetCertificateOffset() const;
    u64 GetTicketOffset() const;
    u64 GetTitleMetadataOffset() const;
    u64 GetMetadataOffset() const;
    u64 GetContentOffset(u16 index = 0) const;

    u32 GetCertificateSize() const;
    u32 GetTicketSize() const;
    u32 GetTitleMetadataSize() const;
    u32 GetMetadataSize() const;
    u64 GetContentSize(u16 index = 0) const;

    void Print() const;

private:
    struct Header {
        u32 header_size;
        u16 type;
        u16 version;
        u32 cert_size;
        u32 tik_size;
        u32 tmd_size;
        u32 meta_size;
        u64 content_size;
        std::array<u8, CIA_CONTENT_INDEX_COUNT> content_index;
    };

    static_assert(sizeof(Header) == CIA_HEADER_SIZE, "CIA Header structure size is wrong");

    struct Metadata {
        std::array<u64, 0x30> dependencies;
        std::array<u8, 0x180> reserved;
        u32 core_version;
        std::array<u8, 0xfc> reserved_2;
    };

    static_assert(sizeof(Metadata) == CIA_METADATA_SIZE, "CIA Metadata structure size is wrong");

    bool loaded = false;
    std::string filepath;
    std::unique_ptr<FileBackend> backend;

    Header cia_header;
    Metadata cia_metadata;
    TitleMetadata cia_tmd;
};

} // namespace FileSys