// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Thomas Chiantia <thomas@altronix>
 * @date 2017
 */

#ifndef TEST_VECTORS_H_
#define TEST_VECTORS_H_

/**
 * @brief
 *
 * RLPX Handshake test vectors
 * https://github.com/ethereum/EIPs/blob/master/EIPS/eip-8.md
 *
 * Note our test vectors skip AUTH_0/ACK_0 eip 8 only
 * @return 0 pass
 */
#define TEST_VECTOR_LEGACY 0
#define TEST_VECTOR_EIP8_0 1
#define TEST_VECTOR_EIP8_1 2
#define TEST_VECTOR_LEGACY_GO 3

#define AUTHVER_1 4
#define AUTHVER_2 4
#define AUTHVER_3 56
#define ACKVER_1 4
#define ACKVER_2 4
#define ACKVER_3 57

#define AUTH_1                                                                 \
    "048ca79ad18e4b0659fab4853fe5bc58eb83992980f4c9cc147d2aa31532efd29a3d3dc6" \
    "a3d89eaf913150cfc777ce0ce4af2758bf4810235f6e6ceccfee1acc6b22c005e9e3a49d" \
    "6448610a58e98744ba3ac0399e82692d67c1f58849050b3024e21a52c9d3b01d871ff5f2" \
    "10817912773e610443a9ef142e91cdba0bd77b5fdf0769b05671fc35f83d83e4d3b0b000" \
    "c6b2a1b1bba89e0fc51bf4e460df3105c444f14be226458940d6061c296350937ffd5e3a" \
    "caceeaaefd3c6f74be8e23e0f45163cc7ebd76220f0128410fd05250273156d548a41444" \
    "4ae2f7dea4dfca2d43c057adb701a715bf59f6fb66b2d1d20f2c703f851cbf5ac47396d9" \
    "ca65b6260bd141ac4d53e2de585a73d1750780db4c9ee4cd4d225173a4592ee77e2bd94d" \
    "0be3691f3b406f9bba9b591fc63facc016bfa8"

#define AUTH_2                                                                 \
    "01b304ab7578555167be8154d5cc456f567d5ba302662433674222360f08d5f1"         \
    "534499d3678b513b0fca474f3a514b18e75683032eb63fccb16c156dc6eb2c0b"         \
    "1593f0d84ac74f6e475f1b8d56116b849634a8c458705bf83a626ea0384d4d73"         \
    "41aae591fae42ce6bd5c850bfe0b999a694a49bbbaf3ef6cda61110601d3b4c0"         \
    "2ab6c30437257a6e0117792631a4b47c1d52fc0f8f89caadeb7d02770bf999cc"         \
    "147d2df3b62e1ffb2c9d8c125a3984865356266bca11ce7d3a688663a51d82de"         \
    "faa8aad69da39ab6d5470e81ec5f2a7a47fb865ff7cca21516f9299a07b1bc63"         \
    "ba56c7a1a892112841ca44b6e0034dee70c9adabc15d76a54f443593fafdc3b2"         \
    "7af8059703f88928e199cb122362a4b35f62386da7caad09c001edaeb5f8a06d"         \
    "2b26fb6cb93c52a9fca51853b68193916982358fe1e5369e249875bb8d0d0ec3"         \
    "6f917bc5e1eafd5896d46bd61ff23f1a863a8a8dcd54c7b109b771c8e61ec9c8"         \
    "908c733c0263440e2aa067241aaa433f0bb053c7b31a838504b148f570c0ad62"         \
    "837129e547678c5190341e4f1693956c3bf7678318e2d5b5340c9e488eefea19"         \
    "8576344afbdf66db5f51204a6961a63ce072c8926c"

#define AUTH_3                                                                 \
    "01b8044c6c312173685d1edd268aa95e1d495474c6959bcdd10067ba4c9013df"         \
    "9e40ff45f5bfd6f72471f93a91b493f8e00abc4b80f682973de715d77ba3a005"         \
    "a242eb859f9a211d93a347fa64b597bf280a6b88e26299cf263b01b8dfdb7122"         \
    "78464fd1c25840b995e84d367d743f66c0e54a586725b7bbf12acca27170ae32"         \
    "83c1073adda4b6d79f27656993aefccf16e0d0409fe07db2dc398a1b7e8ee93b"         \
    "cd181485fd332f381d6a050fba4c7641a5112ac1b0b61168d20f01b479e19adf"         \
    "7fdbfa0905f63352bfc7e23cf3357657455119d879c78d3cf8c8c06375f3f7d4"         \
    "861aa02a122467e069acaf513025ff196641f6d2810ce493f51bee9c966b15c5"         \
    "043505350392b57645385a18c78f14669cc4d960446c17571b7c5d725021babb"         \
    "cd786957f3d17089c084907bda22c2b2675b4378b114c601d858802a55345a15"         \
    "116bc61da4193996187ed70d16730e9ae6b3bb8787ebcaea1871d850997ddc08"         \
    "b4f4ea668fbf37407ac044b55be0908ecb94d4ed172ece66fd31bfdadf2b97a8"         \
    "bc690163ee11f5b575a4b44e36e2bfb2f0fce91676fd64c7773bac6a003f481f"         \
    "ddd0bae0a1f31aa27504e2a533af4cef3b623f4791b2cca6d490"

#define ACK_1                                                                  \
    "049f8abcfa9c0dc65b982e98af921bc0ba6e4243169348a236abe9df5f93aa69d99caddd" \
    "aa387662b0ff2c08e9006d5a11a278b1b3331e5aaabf0a32f01281b6f4ede0e09a2d5f58" \
    "5b26513cb794d9635a57563921c04a9090b4f14ee42be1a5461049af4ea7a7f49bf4c97a" \
    "352d39c8d02ee4acc416388c1c66cec761d2bc1c72da6ba143477f049c9d2dde846c252c" \
    "111b904f630ac98e51609b3b1f58168ddca6505b7196532e5f85b259a20c45e197949168" \
    "3fee108e9660edbf38f3add489ae73e3dda2c71bd1497113d5c755e942d1"

#define ACK_2                                                                  \
    "01ea0451958701280a56482929d3b0757da8f7fbe5286784beead59d95089c217"        \
    "c9b917788989470b0e330cc6e4fb383c0340ed85fab836ec9fb8a49672712aeab"        \
    "bdfd1e837c1ff4cace34311cd7f4de05d59279e3524ab26ef753a0095637ac88f"        \
    "2b499b9914b5f64e143eae548a1066e14cd2f4bd7f814c4652f11b254f8a2d019"        \
    "1e2f5546fae6055694aed14d906df79ad3b407d94692694e259191cde171ad542"        \
    "fc588fa2b7333313d82a9f887332f1dfc36cea03f831cb9a23fea05b33deb999e"        \
    "85489e645f6aab1872475d488d7bd6c7c120caf28dbfc5d6833888155ed69d34d"        \
    "bdc39c1f299be1057810f34fbe754d021bfca14dc989753d61c413d261934e1a9"        \
    "c67ee060a25eefb54e81a4d14baff922180c395d3f998d70f46f6b58306f96962"        \
    "7ae364497e73fc27f6d17ae45a413d322cb8814276be6ddd13b885b201b943213"        \
    "656cde498fa0e9ddc8e0b8f8a53824fbd82254f3e2c17e8eaea009c38b4aa0a3f"        \
    "306e8797db43c25d68e86f262e564086f59a2fc60511c42abfb3057c247a8a8fe"        \
    "4fb3ccbadde17514b7ac8000cdb6a912778426260c47f38919a91f25f4b5ffb45"        \
    "5d6aaaf150f7e5529c100ce62d6d92826a71778d809bdf60232ae21ce8a437eca"        \
    "8223f45ac37f6487452ce626f549b3b5fdee26afd2072e4bc75833c2464c80524"        \
    "6155289f4"

#define ACK_3                                                                  \
    "01f004076e58aae772bb101ab1a8e64e01ee96e64857ce82b1113817c6cdd52c09"       \
    "d26f7b90981cd7ae835aeac72e1573b8a0225dd56d157a010846d888dac7464baf"       \
    "53f2ad4e3d584531fa203658fab03a06c9fd5e35737e417bc28c1cbf5e5dfc666d"       \
    "e7090f69c3b29754725f84f75382891c561040ea1ddc0d8f381ed1b9d0d4ad2a0e"       \
    "c021421d847820d6fa0ba66eaf58175f1b235e851c7e2124069fbc202888ddb3ac"       \
    "4d56bcbd1b9b7eab59e78f2e2d400905050f4a92dec1c4bdf797b3fc9b2f8e84a4"       \
    "82f3d800386186712dae00d5c386ec9387a5e9c9a1aca5a573ca91082c7d68421f"       \
    "388e79127a5177d4f8590237364fd348c9611fa39f78dcdceee3f390f07991b7b4"       \
    "7e1daa3ebcb6ccc9607811cb17ce51f1c8c2c5098dbdd28fca547b3f58c01a424a"       \
    "c05f869f49c6a34672ea2cbbc558428aa1fe48bbfd61158b1b735a65d99f21e70d"       \
    "bc020bfdface9f724a0d1fb5895db971cc81aa7608baa0920abb0a565c9c436e2f"       \
    "d13323428296c86385f2384e408a31e104670df0791d93e743a3a5194ee6b076fb"       \
    "6323ca593011b7348c16cf58f66b9633906ba54a2ee803187344b394f75dd2e663"       \
    "a57b956cb830dd7a908d4f39a2336a61ef9fda549180d4ccde21514d117b6c6fd0"       \
    "7a9102b5efe710a32af4eeacae2cb3b1dec035b9593b48b9d3ca4c13d245d5f041"       \
    "69b0b1"

#define ALICE_SPRI                                                             \
    "49a7b37aa6f6645917e7b807e9d1c00d4fa71f18343b0d4122a4d2df64dd6fee"

#define ALICE_EPRI                                                             \
    "869d6ecf5211f1cc60418a13b9d870b22959d0c16f02bec714c960dd2298a32d"

#define ALICE_SPUB                                                             \
    "fda1cff674c90c9a197539fe3dfb53086ace64f83ed7c6eabec741f7f381cc803e52ab2c" \
    "d55d5569bce4347107a310dfd5f88a010cd2ffd1005ca406f1842877"

#define ALICE_EPUB                                                             \
    "654d1044b69c577a44e5f01a1209523adb4026e70c62d1c13"                        \
    "a067acabc09d2667a49821a0ad4b634554d330a15a58fe61f"                        \
    "8a8e0544b310c6de7b0c8da7528a8d"

#define BOB_SPRI                                                               \
    "b71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291"

#define BOB_EPRI                                                               \
    "e238eb8e04fee6511ab04c6dd3c89ce097b11f25d584863ac2b6d5b35b1847e4"

#define BOB_SPUB                                                               \
    "ca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd31387574077f" \
    "301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f"

#define BOB_EPUB                                                               \
    "b6d82fa3409da933dbf9cb0140c5dde89f4e64aec88d476af64"                      \
    "8880f4a10e1e49fe35ef3e69e93dd300b4797765a747c6384a6"                      \
    "ecf5db9c2690398607a86181e4"

#define NONCE_A                                                                \
    "7e968bba13b6c50e2c4cd7f241cc0d64d1ac25c7f5952df231ac6a2bda8ee5d6"

#define NONCE_B                                                                \
    "559aead08264d5795d3909718cdd05abd49572e84fe55590eef31a88a08fdffd"

#define AES_SECRET                                                             \
    "80e8632c05fed6fc2a13b0f8d31a3cf645366239170ea067065aba8e28bac487"

#define MAC_SECRET                                                             \
    "2ea74ec5dae199227dff1af715362700e989d889d7a493cb0639691efb8e5f98"

#define FOO_SECRET                                                             \
    "0c7ec6340062cc46f5e9f1e3cf86f8c8c403c5a0964f5df0ebd34a75ddc86db5"

// GO Ethereum Test Vectors (pre-eip8)
#define GO_AES_SECRET                                                          \
    "c0458fa97a5230830e05f4f20b7c755c1d4e54b1ce5cf43260bb191eef4e418d"

#define GO_MAC_SECRET                                                          \
    "48c938884d5067a1598272fcddaa4b833cd5e7d92e8228c0ecdfabbe68aef7f1"

#define GO_FOO_SECRET NULL

#define HELLO_PACKET                                                           \
    "6ef23fcf1cec7312df623f9ae701e63b550cdb8517fefd8dd398fc2acd1d935e6e0434a2" \
    "b96769078477637347b7b01924fff9ff1c06df2f804df3b0402bbb9f87365b3c6856b45e" \
    "1e2b6470986813c3816a71bff9d69dd297a5dbd935ab578f6e5d7e93e4506a44f307c332" \
    "d95e8a4b102585fd8ef9fc9e3e055537a5cec2e9"

#define AUTH_GO                                                                \
    "04a0274c5951e32132e7f088c9bdfdc76c9d91f0dc6078e848f8e3361193dbdc43b94351" \
    "ea3d89e4ff33ddcefbc80070498824857f499656c4f79bbd97b6c51a514251d69fd1785e" \
    "f8764bd1d262a883f780964cce6a14ff206daf1206aa073a2d35ce2697ebf3514225bef1" \
    "86631b2fd2316a4b7bcdefec8d75a1025ba2c5404a34e7795e1dd4bc01c6113ece07b0df" \
    "13b69d3ba654a36e35e69ff9d482d88d2f0228e7d96fe11dccbb465a1831c7d4ad3a0269" \
    "24b182fc2bdfe016a6944312021da5cc459713b13b86a686cf34d6fe6615020e4acf26bf" \
    "0d5b7579ba813e7723eb95b3cef9942f01a58bd61baee7c9bdd438956b426a4ffe238e61" \
    "746a8c93d5e10680617c82e48d706ac4953f5e1c4c4f7d013c87d34a06626f498f34576d" \
    "c017fdd3d581e83cfd26cf125b6d2bda1f1d56"

#define ACK_GO                                                                 \
    "049934a7b2d7f9af8fd9db941d9da281ac9381b5740e1f64f7092f3588d4f87f5ce55191" \
    "a6653e5e80c1c5dd538169aa123e70dc6ffc5af1827e546c0e958e42dad355bcc1fcb9cd" \
    "f2cf47ff524d2ad98cbf275e661bf4cf00960e74b5956b799771334f426df007350b4604" \
    "9adb21a6e78ab1408d5e6ccde6fb5e69f0f4c92bb9c725c02f99fa72b9cdc8dd53cff089" \
    "e0e73317f61cc5abf6152513cb7d833f09d2851603919bf0fbe44d79a09245c6e8338eb5" \
    "02083dc84b846f2fee1cc310d2cc8b1b9334728f97220bb799376233e113"

#define ALICE_SPRI_GO                                                          \
    "5e173f6ac3c669587538e7727cf19b782a4f2fda07c1eaa662c593e5e85e3051"

#define ALICE_EPRI_GO                                                          \
    "19c2185f4f40634926ebed3af09070ca9e029f2edd5fae6253074896205f5f6c"

#define BOB_SPRI_GO                                                            \
    "c45f950382d542169ea207959ee0220ec1491755abe405cd7498d6b16adb6df8"

#define BOB_EPRI_GO                                                            \
    "d25688cf0ab10afa1a0e2dba7853ed5f1e5bf1c631757ed4e103b593ff3f5620"

#define ALICE_NONCE_GO                                                         \
    "cd26fecb93657d1cd9e9eaf4f8be720b56dd1d39f190c4e1c6b7ec66f077bb11"

#define BOB_NONCE_GO                                                           \
    "f37ec61d84cea03dcc5e8385db93248584e8af4b4d1c832d8c7453c0089687a7"

#define UDP_TEST_PORT 33

#define DISCOVERY_PRIVKEY                                                      \
    "b71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291"

#define DISCOVERY_PING_V4                                                      \
    "e9614ccfd9fc3e74360018522d30e1419a143407ffcce748de3e22116b7e8dc92ff74788" \
    "c0b6663aaa3d67d641936511c8f8d6ad8698b820a7cf9e1be7155e9a241f556658c55428" \
    "ec0563514365799a4be2be5a685a80971ddcfa80cb422cdd0101ec04cb847f000001820c" \
    "fa8215a8d790000000000000000000000000000000018208ae820d058443b9a3550102"

#define DISCOVERY_PING_V555                                                    \
    "577be4349c4dd26768081f58de4c6f375a7a22f3f7adda654d1428637412c3d7fe917cad" \
    "c56d4e5e7ffae1dbe3efffb9849feb71b262de37977e7c7a44e677295680e9e38ab26bee" \
    "2fcbae207fba3ff3d74069a50b902a82c9903ed37cc993c50001f83e82022bd79020010d" \
    "b83c4d001500000000abcdef12820cfa8215a8d79020010db885a308d313198a2e037073" \
    "488208ae82823a8443b9a355c5010203040531b9019afde696e582a78fa8d95ea13ce329" \
    "7d4afb8ba6433e4154caa5ac6431af1b80ba76023fa4090c408f6b4bc3701562c031041d" \
    "4702971d102c9ab7fa5eed4cd6bab8f7af956f7d565ee1917084a95398b6a21eac920fe3" \
    "dd1345ec0a7ef39367ee69ddf092cbfe5b93e5e568ebc491983c09c76d922dc3"

#define DISCOVERY_PONG                                                         \
    "09b2428d83348d27cdf7064ad9024f526cebc19e4958f0fdad87c15eb598dd61d08423e0" \
    "bf66b2069869e1724125f820d851c136684082774f870e614d95a2855d000f05d1648b2d" \
    "5945470bc187c2d2216fbe870f43ed0909009882e176a46b0102f846d79020010db885a3" \
    "08d313198a2e037073488208ae82823aa0fbc914b16819237dcd8801d7e53f69e9719ade" \
    "cb3cc0e790c57e91ca4461c9548443b9a355c6010203c2040506a0c969a58f6f9095004c" \
    "0177a6b47f451530cab38966a25cca5cb58f055542124e"

#define DISCOVERY_FIND_NODE                                                    \
    "c7c44041b9f7c7e41934417ebac9a8e1a4c6298f74553f2fcfdcae6ed6fe53163eb3d2b5" \
    "2e39fe91831b8a927bf4fc222c3902202027e5e9eb812195f95d20061ef5cd31d502e47e" \
    "cb61183f74a504fe04c51e73df81f25c4d506b26db4517490103f84eb840ca634cae0d49" \
    "acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd31387574077f301b421bc84d" \
    "f7266c44e9e6d569fc56be00812904767bf5ccd1fc7f8443b9a35582999983999999280d" \
    "c62cc8255c73471e0a61da0c89acdc0e035e260add7fc0c04ad9ebf3919644c91cb247af" \
    "fc82b69bd2ca235c71eab8e49737c937a2c396"

#define DISCOVERY_NEIGHBOURS                                                   \
    "c679fc8fe0b8b12f06577f2e802d34f6fa257e6137a995f6f4cbfc9ee50ed3710faf6e66" \
    "f932c4c8d81d64343f429651328758b47d3dbc02c4042f0fff6946a50f4a49037a72bb55" \
    "0f3a7872363a83e1b9ee6469856c24eb4ef80b7535bcf99c0004f9015bf90150f84d8463" \
    "21163782115c82115db8403155e1427f85f10a5c9a7755877748041af1bcd8d474ec065e" \
    "b33df57a97babf54bfd2103575fa829115d224c523596b401065a97f74010610fce76382" \
    "c0bf32f84984010203040101b840312c55512422cf9b8a4097e9a6ad79402e87a15ae909" \
    "a4bfefa22398f03d20951933beea1e4dfa6f968212385e829f04c2d314fc2d4e255e0d3b" \
    "c08792b069dbf8599020010db83c4d001500000000abcdef12820d05820d05b840386432" \
    "00b172dcfef857492156971f0e6aa2c538d8b74010f8e140811d53b98c765dd2d9612605" \
    "1913f44582e8c199ad7c6d6819e9a56483f637feaac9448aacf8599020010db885a308d3" \
    "13198a2e037073488203e78203e8b8408dcab8618c3253b558d459da53bd8fa68935a719" \
    "aff8b811197101a4b2b47dd2d47295286fc00cc081bb542d760717d1bdd6bec2c37cd72e" \
    "ca367d6dd3b9df738443b9a355010203b525a138aa34383fec3d2719a0"

#endif
