#include <includes.hpp>
#include <decrypt.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>


// Call of Duty: Black Ops 4 (T8)

namespace {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;

    class T8GSCOBJHandler : public GSCOBJHandler {
    public:
        T8GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_GLOBAL | GOHF_INLINE_FUNC_PTR | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_FOREACH_TYPE_T8 | GOHF_SUPPORT_GET_API_SCRIPT) {}

        void DumpHeader(std::ostream& asmout, const GscInfoOption& opt) override {
            auto* data = Ptr<T8GSCOBJ>();
            asmout
                << std::hex
                << "// crc: 0x" << std::hex << data->crc << " (" << std::dec << data->crc << ")\n"
                << std::left << std::setfill(' ')
                << "// size ..... " << std::dec << std::setw(3) << data->script_size << "\n"
                << "// includes . " << std::dec << std::setw(3) << data->include_count << " (offset: 0x" << std::hex << data->include_offset << ")\n"
                << "// strings .. " << std::dec << std::setw(3) << data->string_count << " (offset: 0x" << std::hex << data->string_offset << ")\n"
                << "// dev strs . " << std::dec << std::setw(3) << data->devblock_string_count << " (offset: 0x" << std::hex << data->devblock_string_offset << ")\n"
                << "// exports .. " << std::dec << std::setw(3) << data->exports_count << " (offset: 0x" << std::hex << data->export_table_offset << ")\n"
                << "// imports .. " << std::dec << std::setw(3) << data->imports_count << " (offset: 0x" << std::hex << data->imports_offset << ")\n"
                << "// globals .. " << std::dec << std::setw(3) << data->globalvar_count << " (offset: 0x" << std::hex << data->globalvar_offset << ")\n"
                << "// fixups ... " << std::dec << std::setw(3) << data->fixup_count << " (offset: 0x" << std::hex << data->fixup_offset << ")\n"
                << "// cseg ..... 0x" << std::hex << data->cseg_offset << " + 0x" << std::hex << data->cseg_size << " (0x" << (data->cseg_offset + data->cseg_size) << ")" << "\n"
                << std::right
                << std::flush;

            if (opt.m_test_header) {
                asmout
                    << "// ukn0c .... " << std::dec << data->pad << " / 0x" << std::hex << data->pad << "\n"
                    << "// ukn34 .... " << std::dec << data->ukn34 << " / 0x" << std::hex << data->ukn34 << "\n"
                    << "// ukn50 .... " << std::dec << data->ukn50 << " / 0x" << std::hex << data->ukn50 << "\n"
                    << "// ukn5a .... " << std::dec << (int)data->ukn5a << " / 0x" << std::hex << (int)data->ukn5a << "\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<T8GSCOBJ>();

            // no clue what this thing is doing
            uint64_t* requires_implements_table = reinterpret_cast<uint64_t*>(&data->magic[data->requires_implements_offset]);

            for (size_t i = 0; i < data->requires_implements_count; i++) {
                asmout << "#precache(\"requires_implements\" #\"" << hashutils::ExtractTmp("hash", requires_implements_table[i]) << "\");\n";
            }

            if (data->requires_implements_count) {
                asmout << "\n";
            }

            auto* fixups = reinterpret_cast<T8GSCFixup*>(&data->magic[data->fixup_offset]);

            for (size_t i = 0; i < data->fixup_count; i++) {
                const auto& fixup = fixups[i];
                asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << ";\n";
            }

            if (data->fixup_count) {
                asmout << "\n";
            }
        }

        uint64_t GetName() override {
            return Ptr<T8GSCOBJ>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<T8GSCOBJ>()->exports_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<T8GSCOBJ>()->export_table_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<T8GSCOBJ>()->include_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<T8GSCOBJ>()->include_offset;
        }
        uint16_t GetImportsCount() override {
            return Ptr<T8GSCOBJ>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<T8GSCOBJ>()->imports_offset;
        }
        uint16_t GetGVarsCount() override {
            return Ptr<T8GSCOBJ>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return Ptr<T8GSCOBJ>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<T8GSCOBJ>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<T8GSCOBJ>()->string_offset;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<T8GSCOBJ>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<T8GSCOBJ>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<T8GSCOBJ>()->script_size;
        }
        size_t GetHeaderSize() override {
            return sizeof(T8GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return decrypt::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T8GSCOBJ) && *reinterpret_cast<uint64_t*>(file) == 0x36000a0d43534780;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return 0;
        };

        void SetName(uint64_t name) override {
            Ptr<T8GSCOBJ>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0x36000a0d43534780;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<T8GSCOBJ>()->exports_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<T8GSCOBJ>()->export_table_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<T8GSCOBJ>()->include_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<T8GSCOBJ>()->include_offset = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<T8GSCOBJ>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<T8GSCOBJ>()->imports_offset = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<T8GSCOBJ>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<T8GSCOBJ>()->string_offset = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<T8GSCOBJ>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<T8GSCOBJ>()->devblock_string_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {
            Ptr<T8GSCOBJ>()->globalvar_count = val;
        }
        void SetGVarsOffset(uint32_t val) override {
            Ptr<T8GSCOBJ>()->globalvar_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            Ptr<T8GSCOBJ>()->script_size = val;
        }
        void SetCSEGOffset(uint16_t val) override {
            Ptr<T8GSCOBJ>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<T8GSCOBJ>()->cseg_size = val;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {}
        void SetAnimTreeDoubleOffset(uint32_t val) override {}
        size_t GetImportSize() override {
            return sizeof(tool::gsc::T8GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::T8GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return sizeof(tool::gsc::T8GSCGlobalVar);
        }
        size_t GetAnimTreeSingleSize() override {
            return 0;
        }
        size_t GetAnimTreeDoubleSize() override {
            return 0;
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCExport*>(data);
            imp.name = (uint32_t)item.name;
            imp.name_space = (uint32_t)item.name_space;
            imp.callback_event = (uint32_t)item.file_name_space;
            imp.checksum = (uint32_t)item.checksum;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCImport*>(data);
            imp.name = (uint32_t)item.name;
            imp.import_namespace = (uint32_t)item.name_space;
            imp.flags = item.flags;
            imp.num_address = item.num_address;
            imp.param_count = item.param_count;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {
            *reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(data) = item;
        }
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override { }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override { }
        int64_t GetDefaultChecksum(bool client) override {
            // todo: check client
            constexpr int32_t t = 0x636e9d38;
            return t;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<T8GSCOBJ>()->crc = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<T8GSCOBJ>()->crc;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "";
            }
            return "scripts/core_common/clientids_shared.gsc";
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == CLASS_VTABLE;
        }
        byte GetVTableImportFlags() override {
            return CLASS_VTABLE;
        }
    };

    /*****************************************************************************************************************************/

    class T831GSCOBJHandler : public GSCOBJHandler {
    public:
        T831GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_GLOBAL | GOHF_INLINE_FUNC_PTR | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_FOREACH_TYPE_T8 | GOHF_SUPPORT_GET_API_SCRIPT | GOHF_STRING_NAMES) {}

        void DumpHeader(std::ostream& asmout, const GscInfoOption& opt) override {
            auto* data = Ptr<T831GSCOBJ>();
            asmout
                << std::hex
                << "// crc: 0x" << std::hex << data->source_crc << " (" << std::dec << data->source_crc << ")\n"
                << std::left << std::setfill(' ')
                << "// includes . " << std::dec << std::setw(3) << data->include_count << " (offset: 0x" << std::hex << data->include_offset << ")\n"
                << "// strings .. " << std::dec << std::setw(3) << data->stringtablefixup_count << " (offset: 0x" << std::hex << data->stringtablefixup_offset << ")\n"
                << "// dev strs . " << std::dec << std::setw(3) << data->devblock_stringtablefixup_count << " (offset: 0x" << std::hex << data->devblock_stringtablefixup_offset << ")\n"
                << "// exports .. " << std::dec << std::setw(3) << data->exports_count << " (offset: 0x" << std::hex << data->exports_offset << ")\n"
                << "// imports .. " << std::dec << std::setw(3) << data->imports_count << " (offset: 0x" << std::hex << data->imports_offset << ")\n"
                << "// globals .. " << std::dec << std::setw(3) << data->globalvar_count << " (offset: 0x" << std::hex << data->globalvar_offset << ")\n"
                << "// fixups ... " << std::dec << std::setw(3) << data->fixup_count << " (offset: 0x" << std::hex << data->fixup_offset << ")\n"
                << "// animtree . " << std::dec << std::setw(3) << data->animtree_count << " (offset: 0x" << std::hex << data->animtree_offset << ")\n"
                << "// profile .. " << std::dec << std::setw(3) << data->profile_count << " (offset: 0x" << std::hex << data->profile_offset << ")\n"
                << "// cseg ..... 0x" << std::hex << data->cseg_offset << " + 0x" << std::hex << data->cseg_size << " (0x" << (data->cseg_offset + data->cseg_size) << ")" << "\n"
                << std::right
                << std::flush;
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<T831GSCOBJ>();

            auto* fixups = reinterpret_cast<T8GSCFixup*>(&data->magic[data->fixup_offset]);

            for (size_t i = 0; i < data->fixup_count; i++) {
                const auto& fixup = fixups[i];
                asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << ";\n";
            }

            if (data->fixup_count) {
                asmout << "\n";
            }
        }

        uint64_t GetName() override {
            return Ptr<T831GSCOBJ>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<T831GSCOBJ>()->exports_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<T831GSCOBJ>()->exports_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<T831GSCOBJ>()->include_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<T831GSCOBJ>()->include_offset;
        }
        uint16_t GetImportsCount() override {
            return Ptr<T831GSCOBJ>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<T831GSCOBJ>()->imports_offset;
        }
        uint16_t GetGVarsCount() override {
            return Ptr<T831GSCOBJ>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return Ptr<T831GSCOBJ>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<T831GSCOBJ>()->stringtablefixup_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<T831GSCOBJ>()->stringtablefixup_offset;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<T831GSCOBJ>()->devblock_stringtablefixup_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<T831GSCOBJ>()->devblock_stringtablefixup_offset;
        }
        uint32_t GetFileSize() override {
            return (uint32_t)fileSize;
        }
        size_t GetHeaderSize() override {
            return sizeof(T831GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return str;
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T831GSCOBJ) && *reinterpret_cast<uint64_t*>(file) == 0x31000a0d43534780;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return (uint16_t)Ptr<T831GSCOBJ>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<T831GSCOBJ>()->animtree_offset;
        };

        void SetName(uint64_t name) override {
            throw std::runtime_error("can't get string name for this vm");
        }
        void SetNameString(uint32_t name) override {
            Ptr<T831GSCOBJ>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0x31000a0d43534780;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<T831GSCOBJ>()->exports_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<T831GSCOBJ>()->exports_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            if (val > UINT8_MAX) {
                throw std::runtime_error(utils::va("too many includes, max: %d", UINT8_MAX));
            }
            Ptr<T831GSCOBJ>()->include_count = (byte)val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<T831GSCOBJ>()->include_offset = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<T831GSCOBJ>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<T831GSCOBJ>()->imports_offset = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<T831GSCOBJ>()->stringtablefixup_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<T831GSCOBJ>()->stringtablefixup_offset = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<T831GSCOBJ>()->devblock_stringtablefixup_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<T831GSCOBJ>()->devblock_stringtablefixup_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {
            Ptr<T831GSCOBJ>()->globalvar_count = val;
        }
        void SetGVarsOffset(uint32_t val) override {
            Ptr<T831GSCOBJ>()->globalvar_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            Ptr<T831GSCOBJ>()->profile_offset = val;
        }
        void SetCSEGOffset(uint16_t val) override {
            Ptr<T831GSCOBJ>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<T831GSCOBJ>()->cseg_size = val;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {
            if (val > UINT8_MAX) {
                throw std::runtime_error(utils::va("too many animtrees, max: %d", UINT8_MAX));
            }
            Ptr<T831GSCOBJ>()->animtree_count = (byte)val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<T831GSCOBJ>()->animtree_offset = val;
        }
        size_t GetImportSize() override {
            return sizeof(tool::gsc::T8GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::T8GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return sizeof(tool::gsc::T8GSCGlobalVar);
        }
        size_t GetAnimTreeSingleSize() override {
            return 0;
        }
        size_t GetAnimTreeDoubleSize() override {
            return 0;
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCExport*>(data);
            imp.name = (uint32_t)item.name;
            imp.name_space = (uint32_t)item.name_space;
            imp.callback_event = (uint32_t)item.file_name_space;
            imp.checksum = (uint32_t)item.checksum;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCImport*>(data);
            imp.name = (uint32_t)item.name;
            imp.import_namespace = (uint32_t)item.name_space;
            imp.flags = item.flags;
            imp.num_address = item.num_address;
            imp.param_count = item.param_count;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {
            *reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(data) = item;
        }
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override { }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override { }
        int64_t GetDefaultChecksum(bool client) override {
            // todo: check client
            constexpr int32_t t = 0;
            return t;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<T831GSCOBJ>()->source_crc = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<T831GSCOBJ>()->source_crc;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "";
            }
            return "scripts/core_common/clientids_shared.gsc";
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == CLASS_VTABLE;
        }
        byte GetVTableImportFlags() override {
            return CLASS_VTABLE;
        }
    };

}
REGISTER_GSC_VM(VMI_T8, T8GSCOBJHandler);
REGISTER_GSC_VM(VMI_T831, T831GSCOBJHandler);