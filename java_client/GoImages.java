/*
 * Shamelessly ripped from qgo.
 */

import java.awt.Image;
import java.awt.image.BufferedImage;

class GoImages {
	private class WhiteDesc {
		public double cosTheta, sinTheta;
		public double stripeWidth, xAdd;
		public double stripeMul, zMul;
	}
//--------------------------------------------//
	static final int D_WHITE_LOOKS = 16;
	public Image _background;
	public Image _shadow;
	public Image _black;
	public Image _blackAlpha;
	public Image[] _whites;
	public Image[] _whitesAlpha;
	private int _stoneSize;
//--------------------------------------------//
	public GoImages() {
		_whites = new Image[D_WHITE_LOOKS];
		_whitesAlpha = new Image[D_WHITE_LOOKS];
	}
	public void regenerate( int $gobanSize, int $stoneSize ) {
		_stoneSize = $stoneSize;
		_background = background( $gobanSize );
		_black = black( $stoneSize, false );
		_blackAlpha = black( $stoneSize, true );
		_shadow = shadow( $stoneSize );
		for ( int i = 0; i < D_WHITE_LOOKS; ++ i ) {
			_whites[ i ] = white( $stoneSize, false );
			_whitesAlpha[ i ] = white( $stoneSize, true );
		}
	}
	public int getStoneSize() {
		return ( _stoneSize );
	}
	private Image background( int $size ) {
		final int D_WIDTH = $size;
		final int D_HEIGHT = $size;
		BufferedImage img = new BufferedImage( D_WIDTH, D_HEIGHT, BufferedImage.TYPE_INT_RGB );
		int[] buf = new int[ $size * $size ];
		for ( int y = 0; y < D_HEIGHT; ++ y ) {
			for ( int x = 0; x < D_WIDTH; ++ x ) {
				int vx = x - D_WIDTH * 8;
				int vy = y - D_WIDTH / 2;
				int vc = (int)( Math.sin( Math.sqrt( vx * vx + vy * vy ) ) * 8. );
				int vcr = 236 + vc;
				int vcg = 166 + vc;
				if ( vcr > 255 ) {
					vcr = 255;
				}
				if ( vcg > 255 ) {
					vcg = 255;
				}
				buf[ y * $size + x ] = ( vcr << 16 ) | ( vcg << 8 ) | 90;
			}
		}
		img.setRGB( 0, 0, $size, $size, buf, 0, $size );
		return ( img );
	}

	private void decideAppearance( WhiteDesc $desc, int $size ) {
		double minStripeW = (double)$size / 20.0;
		if (minStripeW < 1.5) {
			minStripeW = 1.5;
		}

		double maxStripeW = (double)$size / 10.0;
		if ( maxStripeW < 2.5 ) {
			maxStripeW = 2.5;
		}

		double theta = Math.random() * 2.0 * Math.PI;
		$desc.cosTheta = Math.cos( theta );
		$desc.sinTheta = Math.sin( theta );
		$desc.stripeWidth = 1.5 * minStripeW + ( Math.random() * ( maxStripeW - minStripeW ) );

		$desc.xAdd = 3 * $desc.stripeWidth + (double)$size * 3.0;

		$desc.stripeMul = 3.0;
		$desc.zMul = Math.random() * 650.0 + 70.0;
	}

	double getStripe( WhiteDesc white, double bright, double z, int x, int y ) {
		bright /= 256.0;

		double wStripeLoc = x * white.cosTheta - y * white.sinTheta +	white.xAdd;
		double wStripeColor = Math.IEEEremainder(
			wStripeLoc + ( z * z * z * white.zMul ) *
			white.stripeWidth,
			white.stripeWidth
		) / white.stripeWidth;
		wStripeColor = wStripeColor * white.stripeMul - 0.5;
		if ( wStripeColor < 0.0 ) {
			wStripeColor = -2.0 * wStripeColor;
		}
		if ( wStripeColor > 1.0 ) {
			wStripeColor = 1.0;
		}
		wStripeColor = wStripeColor * 0.15 + 0.85;

		double wBright = 0;
		if ( bright < 0.95 ) {
			wBright = bright * wStripeColor;
		} else {
			wBright = bright * Math.sqrt( Math.sqrt( wStripeColor ) );
		}

		if ( wBright > 255 ) {
			wBright = 255;
		}
		if ( wBright < 0 ) {
			wBright = 0;
		}

		return ( wBright * 255 );
	}

	private Image white( int $size, boolean $alpha ) {
		final int D_WIDTH = $size;
		final int D_HEIGHT = $size;
		WhiteDesc desc = new WhiteDesc();
		decideAppearance( desc, $size );

		// the angle from which the shadow starts (measured to the light direction = pi/4)
		// alpha should be in (0, pi)
		final double ALPHA = Math.PI / 4.0;
		// how big the shadow is (should be < d/2)
		final double STRIPE = $size / 5.0;
		final double pixel = 0.8;

		// these are the images
		int[] pw = new int[ $size * $size ];
		double d2 = (double)$size / 2.0 - 0.5;
		double r = d2 - 0.2;
		double f = Math.sqrt( 3.0 );

		int k = 0;
		for ( int i = 0; i < $size; ++ i ) {
			for ( int j = 0; j < $size; ++ j ) {
				double di = i - d2;
				double dj = j - d2;
				double hh = r - Math.sqrt( di * di + dj * dj );
				if ( hh >= 0 ) {
					double z = r * r - di * di - dj * dj;
					if ( z > 0 ) {
						z = Math.sqrt( z ) * f;
					} else {
						z = 0;
					}
					double xr = Math.sqrt( 6 * ( di * di + dj * dj + z * z ) );
					double xr1 = ( 2 * z - di + dj ) / xr;
					double xr2 = ( 2 * z + di - dj ) / xr;

					double xg1 = ( xr1 > 0.9 ) ? ( xr1 - 0.9 ) * 10 : 0;
					double xg2 = ( xr2 > 0.92 ) ? ( xr2 - 0.92 ) * 10 : 0;

					double theta = Math.atan2( (double)( j - $size / 2 ), (double)( i - $size / 2 ) ) + Math.PI - Math.PI / 4.0 + Math.PI / 2.0;
					boolean stripeband = theta > ALPHA && theta < 2.0 * Math.PI - ALPHA;

					if ( theta > Math.PI ) {
						theta = ( 2 * Math.PI - theta );
					}

					double stripe = STRIPE * Math.sin( ( Math.PI / 2.0 ) * ( theta - ALPHA ) / ( Math.PI - ALPHA ) );
					if ( stripe < 1 ) {
						stripe = 1;
					}

					double g1min = 0 + 10 * xr1 + xg1 * 45;
					double g2min = 0 + 10 * xr2 + xg2 * 45;
					double g1max = 200 + 10 * xr1 + xg1 * 45;
					double g2max = 200 + 10 * xr2 + xg2 * 45;
					g1min = g1max - ( g1max - g1min ) * ( 1.0 - Math.getExponent( -1.0 * ( theta - ALPHA ) / ( Math.PI - ALPHA ) ) );
					g2min = g2max - ( g2max - g2min ) * ( 1.0 - Math.getExponent( -1.0 * ( theta - ALPHA ) / ( Math.PI - ALPHA ) ) );

					if ( ( hh < STRIPE ) && ( hh > pixel ) && stripeband ) {
						double g1 = g1max;
						double g2 = g2max;
						if ( hh <= stripe ) {
							g1 = g1min + ( g1max - g1min ) * Math.sqrt( hh / stripe );
							g2 = g2min + ( g2max - g2min ) * Math.sqrt( hh / stripe );
						}
						int g = (int)( g1 > g2 ? g1 : g2 );
						g = (int)getStripe( desc, g, xr1 / 7.0, i, j );
						pw[k] = ( ( $alpha ? 120 : 255 ) << 24 ) | ( g << 16 ) | ( g << 8 ) | g;
					} else if ( hh > pixel ) {
						int g = (int)( g1max > g2max ? g1max : g2max );
						g = (int)getStripe( desc, g, xr1 / 7.0, i, j );
						pw[k] = ( ( $alpha ? 120 : 255 ) << 24 ) | ( g << 16 ) | ( g << 8 ) | g;
					} else {

						int g1 = (int)( stripeband ? g1min : g1max );
						int g2 = (int)( stripeband ? g2min : g2max );

						int g = ( g1 > g2 ? g1 : g2 );
						g = (int)getStripe( desc, g, xr1 / 7.0, i, j );

						pw[k] = ((int)( hh / pixel * ( $alpha ? 120 : 255 ) ) << 24 ) | ( g << 16 ) | ( g << 8 ) | g;
					}
				} else {
					pw[k] = 0;
				}
				++ k;
			}
		}
		BufferedImage img = new BufferedImage( D_WIDTH, D_HEIGHT, BufferedImage.TYPE_INT_ARGB );
		img.setRGB( 0, 0, $size, $size, pw, 0, $size );
		return ( img );
	}

	private Image black( int $size, boolean $alpha ) {
		final int D_WIDTH = $size;
		final int D_HEIGHT = $size;
		final double pixel = 0.8;

		// these are the images
		int[] pb = new int[$size*$size];
		double d2 = (double)$size / 2.0 - 0.5;
		double r = d2 - 0.2;
		double f = Math.sqrt( 3 );

		int k = 0;
		for ( int i = 0; i < $size; ++ i ) {
			for ( int j = 0; j < $size; ++ j ) {
				double di = i - d2;
				double dj = j - d2;
				double hh = r - Math.sqrt( di * di + dj * dj );
				if ( hh >= 0 ) {
					double z = r * r - di * di - dj * dj;
					if ( z > 0 ) {
						z = Math.sqrt( z ) * f;
					} else {
						z = 0;
					}
					double xr = Math.sqrt( 6 * ( di * di + dj * dj + z * z ) );
					double xr1 = ( 2 * z - di + dj ) / xr;
					//double xr2 = ( 1.6 * z + di + 1.75 * dj ) / xr;
					double xr2 = ( 2 * z + di - dj ) / xr;

					double xg1 = ( xr1 > 0.9 ) ? ( xr1 - 0.9 ) * 10 : 0;
					double xg2 = ( xr2 > 0.96 ) ? ( xr2 - 0.96 ) * 10 : 0;

					int g1 = (int)( 5 + 10 * Math.random() + 10 * xr1 + xg1 * 140 );
					int g2 = (int)( 10 + 10 * xr2 + xg2 * 160 );
					int g = ( g1 > g2 ? g1 : g2 );
					// int = g = (int) 1 / ( 1 / g1 + 1 / g2 );

					if ( hh > pixel ) {
						pb[k] = ( ( $alpha ? 120 : 255 ) << 24 ) | ( g << 16 ) | ( g << 8 ) | g;
					} else {
						pb[k] = ( (int)( hh / pixel * ( $alpha ? 120 : 255 ) ) << 24 ) | ( g << 16 ) | ( g << 8) | g;
					}

				} else {
					pb[k] = 0;
				}
				++ k;
			}
		}
		BufferedImage img = new BufferedImage( D_WIDTH, D_HEIGHT, BufferedImage.TYPE_INT_ARGB );
		img.setRGB( 0, 0, $size, $size, pb, 0, $size );
		return ( img );
	}

	Image shadow( int $size ) {
		final int D_WIDTH = $size;
		final int D_HEIGHT = $size;

		// these are the images
		int[] pw = new int[ $size * $size ];
		double d2 = (double)$size / 2.0 - 0.5;
		double r = d2 - 0.2;
		int k = 0;
		for ( int i = 0; i < $size; ++ i ) {
			for ( int j = 0; j < $size; ++ j ) {
				double di = i - d2;
				double dj = j - d2;
				double hh = r - Math.sqrt( di * di + dj * dj );
				if ( hh >= 0 ) {
					hh = 2 * hh / r;
					if ( hh > 1 ) {
						hh = 1;
					}
					pw[k] = ((int)( 255 * hh ) << 24 ) | ( 1 << 16 ) | ( 1 << 8 ) | 1;
				} else {
					pw[k] = 0;
				}
				++ k;
			}
		}

		BufferedImage img = new BufferedImage( D_WIDTH, D_HEIGHT, BufferedImage.TYPE_INT_ARGB );
		img.setRGB( 0, 0, $size, $size, pw, 0, $size );
		return ( img );
	}
}
