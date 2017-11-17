#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/printk.h>
#include <linux/hrtimer.h>

#include <crypto/hash.h>
#include <crypto/drbg.h>

MODULE_LICENSE ("GPL");
MODULE_AUTHOR  ("nouredd2");
MODULE_DESCRIPTION ("Testing module for Tcp challenges");

struct shash_desc * init_sdesc_from_alg (struct crypto_shash *alg)
{
  struct shash_desc *sdesc;
  int size;

  size = sizeof (struct shash_desc) + crypto_shash_descsize(alg);
  sdesc = (struct shash_desc *) kmalloc (size, GFP_KERNEL);
  sdesc->tfm = alg;
  sdesc->flags = 0x0;

  return sdesc;
}

int run_hashes (int num_hashes)
{
  struct crypto_shash *alg = 0;
  struct shash_desc *sdesc = 0;
  u8 *x, *digest;
  u64 diff;
  ktime_t start, end;
  int i;

  alg = crypto_alloc_shash ("sha256", CRYPTO_ALG_TYPE_DIGEST,
          CRYPTO_ALG_TYPE_HASH_MASK);

  if (IS_ERR(alg)) {
    pr_err("Failed to create algorithm\n");
    return -ENOMEM;
  }

  sdesc = init_sdesc_from_alg(alg);

  /* allocate memory */
  x = (u8 *) kmalloc (256, GFP_KERNEL);
  digest = (u8 *) kmalloc (crypto_shash_digestsize(alg), GFP_KERNEL);


  pr_info ("Hello from challenge kernel module\n");
  start = ktime_get();
  for (i=0; i < num_hashes; ++i) {
    crypto_shash_init (sdesc);
    crypto_shash_update (sdesc, x, 256);
    crypto_shash_final (sdesc, digest);
  }
  end = ktime_get();
  diff = ktime_us_delta(end, start);
  pr_info ("Ran for %lld us\n", diff);


  kfree (x);
  kfree (digest);
  kfree (alg);
  kfree (sdesc);

  return 0;
}

int run_for_usec (u64 usec)
{
  struct crypto_shash *alg = 0;
  struct shash_desc *sdesc = 0;
  u8 *x, *digest;
  ktime_t start, end;
  int num_hashes = 0;

  alg = crypto_alloc_shash ("sha256", CRYPTO_ALG_TYPE_DIGEST,
          CRYPTO_ALG_TYPE_HASH_MASK);

  if (IS_ERR(alg)) {
    pr_err("Failed to create algorithm\n");
    return -ENOMEM;
  }

  sdesc = init_sdesc_from_alg(alg);

  /* allocate memory */
  x = (u8 *) kmalloc (256, GFP_KERNEL);
  digest = (u8 *) kmalloc (crypto_shash_digestsize(alg), GFP_KERNEL);


  pr_info ("Hello from challenge kernel module\n");
  start = ktime_get();
  end = ktime_get();

  while (ktime_us_delta(end, start) <= usec) {
    crypto_shash_init (sdesc);
    crypto_shash_update (sdesc, x, 256);
    crypto_shash_final (sdesc, digest);

    end = ktime_get();
    num_hashes += 1;
  }
  pr_info ("Performed %d hashes in %lld usec.\n", num_hashes, ktime_us_delta(end, start));


  kfree (x);
  kfree (digest);
  kfree (alg);
  kfree (sdesc);

  return 0;
}

static int __init challenge_init (void)
{
  /* 
   * int num_hashes = 327680;
   * return run_hashes (num_hashes); */
  u64 usec = 100000; /* 100 ms */
  return run_for_usec (usec);
}

static void __exit challenge_cleanup (void)
{
  pr_info ("Cleaning up module\n");
}

module_init (challenge_init);
module_exit (challenge_cleanup);
