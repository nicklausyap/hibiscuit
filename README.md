# hibiscuit
**hibiscuit** is a hashing function designed to offer a level of complexity exceeding that of basic hashing functions, yet falling short of the complexity associated with non-cryptographic hash functions.

The inception of the **hibiscuit** project was driven by the necessity for a hashing function capable of processing messages with a length of 255, accommodating various combinations for utilization in another project. **hibiscuit** facilitates the generation of both 192-bit and 96-bit hashes.

### Files

- **main.c**: Contains the source code for **hibiscuit**, yielding a 192-bit hash.
- **main_96bit.c**: Presents the source code tailored to produce a 96-bit hash, aligning with **hibiscuit**'s adaptability.
- **96bit.js**: Presents the js source code tailored to produce a 96-bit hash in javascript.
- **96bit.php**: Presents the php source code tailored to produce a 96-bit hash in php.

It's worth noting that the difference between **hibiscuit**'s 192-bit hash and 96-bit hash lies in the reduction of the number of rounds to 60 for the latter. Additionally, the final hash value for the 96-bit hash is derived from h0, h4, and h5.

It's imperative to note that while **hibiscuit** fulfills its intended purpose effectively, it does not meet cryptographic security standards. Therefore, its application is advised only in scenarios where hash collision risks are minimal, prioritizing efficiency over stringent security protocols.

