#include "operations.h"

/**
 * Cap given value between 0 and 255
 */
uchar vcap(int value) {
    /*if(value > 255) return 255;
    if(value < 0) return 0;
    return value; */
	return saturate_cast<uchar>(value);
}

Mat convertToBGR(Mat img) {
    if (img.type()==CV_8UC1) { // if input image is grayscale
        cvtColor(img, img, CV_GRAY2BGR);
    }
    return img;
}

Mat convertToGS(Mat img) {
    if (img.type()!=CV_8UC1) { // mask is not grayscale
        cvtColor(img, img, CV_BGR2GRAY);
    }
    return img;
}

/**
 * Brightness OK
 */
Mat brightness(Mat img, int dec) {
    img = convertToBGR(img);

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b & color = img.at<Vec3b>(y,x); // get pixel
			color[0] = vcap(color[0] + dec);
			color[1] = vcap(color[1] + dec);
			color[2] = vcap(color[2] + dec);
			img.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	return img;
}

/**
 * Contrast OK
 */
Mat contrast(Mat img, int dec) {
    img = convertToBGR(img);
	Mat imgHsv;
	cvtColor(img, imgHsv, CV_BGR2HSV);

	int min = 255;
	int max = 0;

	for(int y = 0; y < imgHsv.rows; y++) {
		for(int x = 0; x < imgHsv.cols; x++) {
			Vec3b & color = imgHsv.at<Vec3b>(y,x); // get pixel
			if(color[2] < min)
				min = color[2];
			else if(color[2] > max)
				max = color[2];
        }
    }

	int newMax = max + dec;
	int newMin = min - dec;

	for(int y = 0; y < imgHsv.rows; y++) {
		for(int x = 0; x < imgHsv.cols; x++) {
			Vec3b & color = imgHsv.at<Vec3b>(y,x); // get pixel
			color[2] = vcap(static_cast<int>(newMin + (newMax-newMin)*(color[2]-min)/(max-min)));
			imgHsv.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	cvtColor(imgHsv, img, CV_HSV2BGR);
	return img;
}

/**
 * Saturate OK
 */
Mat saturate(Mat img, int dec) {
    img = convertToBGR(img);
	Mat imgHsv;
	cvtColor(img, imgHsv, CV_BGR2HSV);

	for(int y = 0; y < imgHsv.rows; y++) {
		for(int x = 0; x < imgHsv.cols; x++) {
			Vec3b & color = imgHsv.at<Vec3b>(y,x); // get pixel

            color[1] = vcap(color[1] + dec);

			imgHsv.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	cvtColor(imgHsv, img, CV_HSV2BGR);
	return img;
}

/**
 * Threshold OK
 */
Mat threshold(Mat img, int t) {
    img = convertToBGR(img);
    t = vcap(t);

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
            Vec3b & color = img.at<Vec3b>(y,x); // get pixel
			int avg = (color[0] + color[1] + color[2])/3;
			if(avg > t) {
                img.at<Vec3b>(Point(x,y)) = Vec3b(255, 255, 255); // set pixel to white
			}
			else {
				img.at<Vec3b>(Point(x,y)) = Vec3b(0, 0, 0); // set pixel to black
			}
        }
    }
	return img;
}

/**
 * Invert OK
 */
Mat invert(Mat img) {
    img = convertToBGR(img);

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b & color = img.at<Vec3b>(y,x); // get pixel
			color[0] = 255 - color[0];
			color[1] = 255 - color[1];
			color[2] = 255 - color[2];
			img.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	return img;
}

/**
 * Horizontal flip OK
 */
Mat horizontalFlip(Mat img) {
	Mat imgCopy = img.clone();

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			imgCopy.at<Vec3b>(Point(img.cols-1-x, y)) = img.at<Vec3b>(y,x); // set pixel
		}
	}

	return imgCopy;
}

/**
 * Vertical flip OK
 */
Mat verticalFlip(Mat img) {
	Mat imgCopy = img.clone();

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			imgCopy.at<Vec3b>(Point(x, img.rows-1-y)) = img.at<Vec3b>(y,x); // set pixel
		}
	}

	return imgCopy;
}

/**
 * Rotate 90° clockwise OK
 */
Mat rotate90(Mat img) {
	Mat imgCopy = img.clone();

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
            imgCopy.at<Vec3b>(Point(img.rows-y-1, x)) = img.at<Vec3b>(y,x); // set pixel
		}
	}

	return imgCopy;
}

/**
 * Generic rotate OK
 */
Mat rotate(Mat img, float angle, int *center) {
    img = convertToBGR(img);
	Mat imgCopy = img.clone();

    angle = angle * 3.1415926/180; // convert to radians
	float ct = cos(-angle);
	float st = sin(-angle);
	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			int a = center[0]+static_cast<int>(ct*(x-center[0])-st*(y-center[1]));
			int b = center[0]+static_cast<int>(st*(x-center[0])+ct*(y-center[1]));
			if(a >= 0 && a < img.cols && b >= 0 && b < img.rows) {
				imgCopy.at<Vec3b>(Point(x,y)) = img.at<Vec3b>(b,a); // set pixel
			}
			else {
				Vec3b zero(0, 0, 0);
				imgCopy.at<Vec3b>(Point(x,y)) = zero; // set pixel
			}
		}
	}

	return imgCopy;
}

/**
 * Crop OK
 */
Mat crop(Mat img, int top, int left, int bottom, int right) {
	Mat imgCrop(bottom-top, right-left, CV_8UC3, Scalar(255, 255, 255));
    
	for(int y = top; y < bottom; y++) {
		for(int x = left; x < right; x++) {
			imgCrop.at<Vec3b>(Point(x-left,y-top)) = img.at<Vec3b>(y,x);
        }
    }

	return imgCrop;
}

/**
 * Translate OK
 */
Mat translate(Mat img, int *dec) {
	Mat imgCopy = img.clone();

    // handle negative values
    if(dec[0] < 0) dec[0] = img.cols - abs(dec[0]);
    if(dec[1] < 0) dec[1] = img.rows - abs(dec[1]);

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			imgCopy.at<Vec3b>(Point((x+dec[0])%img.cols, (y+dec[1])%img.rows)) = img.at<Vec3b>(y,x);
            //imgCopy.at<Vec3b>(Point(x,y)) = img.at<Vec3b>((y-dec[1])%img.rows, (x-dec[0])%img.cols);
        }
    }

	return imgCopy;
}

/**
 * Resize OK
 */
Mat resize(Mat img, int *dims) {
    Mat newImg(dims[1], dims[0], CV_8UC3, Scalar(255, 255, 255));

	for(int y = 0; y < dims[1]; y++) {
		for(int x = 0; x < dims[0]; x++) {
			newImg.at<Vec3b>(Point(x,y)) = img.at<Vec3b>(static_cast<int>((static_cast<float>(y)/dims[1])*img.rows), static_cast<int>((static_cast<float>(x)/dims[0])*img.cols));
        }
    }

    return newImg;
}

/**
 * Blur OK
 */
Mat blur(Mat img, int radius) {
    img = convertToBGR(img);
	Mat imgCopy = img.clone();

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			int sum_values[3] = {0, 0, 0};
			int num_values = 0;
			for(int i = -radius; i < radius+1; i++) {
				for(int j = -radius; j < radius+1; j++) {
					if(x+i >= 0 && x+i < img.cols && y+j >= 0 && y+j < img.rows) {
						Vec3b & color = img.at<Vec3b>(y+j,x+i);
						for(int c = 0; c < 3; c++)
							sum_values[c] += color[c];
						num_values++;
					}
				}
			}
			for(int c = 0; c < 3; c++)
				sum_values[c] /= num_values;

			Vec3b final(sum_values[0], sum_values[1], sum_values[2]);
			imgCopy.at<Vec3b>(Point(x,y)) = final; // set pixel
		}
	}

	return imgCopy;
}

/**
 * Erode OK
 */
Mat erode(Mat img, int radius) {
    img = convertToBGR(img);
	Mat imgCopy = img.clone();

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b min_val(255, 255, 255);
			for(int i = -radius; i < radius+1; i++) {
				for(int j = -radius; j < radius+1; j++) {
					if(x+i >= 0 && x+i < img.cols && y+j >= 0 && y+j < img.rows) {
						Vec3b & color = img.at<Vec3b>(y+j,x+i);
						for(int c = 0; c < 3; c++)
							min_val[c] = min_val[c] < color[c] ? min_val[c] : color[c];
					}
				}
			}
			imgCopy.at<Vec3b>(Point(x,y)) = min_val; // set pixel
		}
	}

	return imgCopy;
}

/**
 * Dilate OK
 */
Mat dilate(Mat img, int radius) {
    img = convertToBGR(img);
	Mat imgCopy = img.clone();

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b max_val(0, 0, 0);
			for(int i = -radius; i < radius+1; i++) {
				for(int j = -radius; j < radius+1; j++) {
					if(x+i >= 0 && x+i < img.cols && y+j >= 0 && y+j < img.rows) {
						Vec3b & color = img.at<Vec3b>(y+j,x+i);
						for(int c = 0; c < 3; c++)
							max_val[c] = max_val[c] > color[c] ? max_val[c] : color[c];
					}
				}
			}
			imgCopy.at<Vec3b>(Point(x,y)) = max_val; // set pixel
		}
	}

	return imgCopy;
}

/**
 * Normalize OK
 */
Mat* normalize(Mat *images, int images_count) {
	int minW = 100000, maxW = 0, minH = 100000, maxH = 0;

	for(int i = 0; i < images_count; i++) {
		if(images[i].cols < minW) minW = images[i].cols;
		if(images[i].cols > maxW) maxW = images[i].cols;
		if(images[i].rows < minH) minH = images[i].rows;
		if(images[i].rows > maxH) maxH = images[i].rows;
	}
    
	if(minW == maxW && minH == maxH) {
		return images;
	}
	for(int i = 0; i < images_count; i++) {
		images[i] = crop(images[i], 0, 0, minH, minW);
	}

	return images;
}

/**
 * Add OK
 */
Mat add(Mat img1, Mat img2) {
    Mat *images = new Mat[2];
    images[0] = convertToBGR(img1);
    images[1] = convertToBGR(img2);
	images = normalize(images, 2);

	Mat a = images[0], b = images[1];
	Mat img(a.rows, a.cols, CV_8UC3, Scalar(255, 255, 255));

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b & a_color = a.at<Vec3b>(y,x); // get pixel
			Vec3b & b_color = b.at<Vec3b>(y,x); // get pixel

			Vec3b & color = img.at<Vec3b>(y,x);
			color[0] = vcap(a_color[0] + b_color[0]);
			color[1] = vcap(a_color[1] + b_color[1]);
			color[2] = vcap(a_color[2] + b_color[2]);
			img.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	return img;
}

/**
 * Difference OK
 */
Mat diff(Mat img1, Mat img2) {
	Mat *images = new Mat[2];
    images[0] = convertToBGR(img1);
    images[1] = convertToBGR(img2);
	images = normalize(images, 2);

	Mat a = images[0], b = images[1];
	Mat img(a.rows, a.cols, CV_8UC3, Scalar(255, 255, 255));

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b & a_color = a.at<Vec3b>(y,x); // get pixel
			Vec3b & b_color = b.at<Vec3b>(y,x); // get pixel

			Vec3b & color = img.at<Vec3b>(y,x);
			color[0] = vcap(a_color[0] - b_color[0]);
			color[1] = vcap(a_color[1] - b_color[1]);
			color[2] = vcap(a_color[2] - b_color[2]);
			img.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	return img;
}

/**
 * Product OK
 */
Mat product(Mat img1, Mat img2) {
	Mat *images = new Mat[2];
    images[0] = convertToBGR(img1);
    images[1] = convertToBGR(img2);
	images = normalize(images, 2);

	Mat a = images[0], b = images[1];
	Mat img(a.rows, a.cols, CV_8UC3, Scalar(255, 255, 255));
    
	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b & a_color = a.at<Vec3b>(y,x); // get pixel
			Vec3b & b_color = b.at<Vec3b>(y,x); // get pixel

			Vec3b & color = img.at<Vec3b>(y,x);
			color[0] = vcap(a_color[0] * static_cast<float>(b_color[0]) / 255);
			color[1] = vcap(a_color[1] * static_cast<float>(b_color[1]) / 255);
			color[2] = vcap(a_color[2] * static_cast<float>(b_color[2]) / 255);
			img.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	return img;
}

/**
 * Screen OK
 */
Mat screen(Mat img1, Mat img2) {
	Mat *images = new Mat[2];
    images[0] = convertToBGR(img1);
    images[1] = convertToBGR(img2);
	images = normalize(images, 2);

	Mat a = images[0], b = images[1];
	Mat img(a.rows, a.cols, CV_8UC3, Scalar(255, 255, 255));

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b & a_color = a.at<Vec3b>(y,x); // get pixel
			Vec3b & b_color = b.at<Vec3b>(y,x); // get pixel

			Vec3b & color = img.at<Vec3b>(y,x);
			color[0] = vcap(255 - (255-a_color[0]) * (255-b_color[0]) /255);
			color[1] = vcap(255 - (255-a_color[1]) * (255-b_color[1]) /255);
			color[2] = vcap(255 - (255-a_color[2]) * (255-b_color[2]) /255);
			img.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	return img;
}

/**
 * Overlay OK
 */
Mat overlay(Mat img1, Mat img2) {
	Mat *images = new Mat[2];
    images[0] = convertToBGR(img1);
    images[1] = convertToBGR(img2);
	images = normalize(images, 2);

	Mat a = images[0], b = images[1];
	Mat img(a.rows, a.cols, CV_8UC3, Scalar(255, 255, 255));
	Mat img_screen = screen(a, b);
    Mat img_product = product(a, b);

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
            Vec3b & a_color = a.at<Vec3b>(y,x); // get pixel
			int avg = (a_color[0] + a_color[1] + a_color[2])/3;
			if(avg < 128) { // product
                img.at<Vec3b>(Point(x,y)) = img_product.at<Vec3b>(y,x); // set pixel
			}
			else { // screen
				img.at<Vec3b>(Point(x,y)) = img_screen.at<Vec3b>(y,x); // set pixel
			}
        }
    }
	return img;
}

/**
 * Binary merge OK
 */
Mat binaryMerge(Mat img1, Mat img2, Mat mask) {
	Mat *images = new Mat[2];
    images[0] = convertToBGR(img1);
    images[1] = convertToBGR(img2);
	images = normalize(images, 2);

	Mat a = images[0], b = images[1];
	Mat img(a.rows, a.cols, CV_8UC3, Scalar(255, 255, 255));

    mask = convertToGS(mask);
    int *dims = new int[2];
    dims[0] = img.rows;
    dims[1] = img.cols;
    mask = resize(mask, dims);

	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b & a_color = a.at<Vec3b>(y,x); // get pixel
			Vec3b & b_color = b.at<Vec3b>(y,x); // get pixel

			Vec3b & color = img.at<Vec3b>(y,x);
			if(mask.at<uchar>(y,x) == 0) {
				color[0] = a_color[0];
				color[1] = a_color[1];
				color[2] = a_color[2];
			}
			else {
				color[0] = b_color[0];
				color[1] = b_color[1];
				color[2] = b_color[2];
			}
			img.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	return img;
}

/**
 * Weighted merge OK
 */
Mat weightedMerge(Mat img1, Mat img2, Mat mask) {
	Mat *images = new Mat[2];
    images[0] = convertToBGR(img1);
    images[1] = convertToBGR(img2);
	images = normalize(images, 2);

	Mat a = images[0], b = images[1];
	Mat img(a.rows, a.cols, CV_8UC3, Scalar(255, 255, 255));

    mask = convertToGS(mask);
    int *dims = new int[2];
    dims[0] = img.rows;
    dims[1] = img.cols;
    mask = resize(mask, dims);
    
	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			Vec3b & a_color = a.at<Vec3b>(y,x); // get pixel
			Vec3b & b_color = b.at<Vec3b>(y,x); // get pixel

			Vec3b & color = img.at<Vec3b>(y,x);
			color[0] = vcap(a_color[0] * (static_cast<float>(mask.at<uchar>(y,x))/255) + b_color[0] * static_cast<float>(1-(mask.at<uchar>(y,x)/255)));
			color[1] = vcap(a_color[1] * (static_cast<float>(mask.at<uchar>(y,x))/255) + b_color[1] * static_cast<float>(1-(mask.at<uchar>(y,x)/255)));
			color[2] = vcap(a_color[2] * (static_cast<float>(mask.at<uchar>(y,x))/255) + b_color[2] * static_cast<float>(1-(mask.at<uchar>(y,x)/255)));
			img.at<Vec3b>(Point(x,y)) = color; // set pixel
        }
    }

	return img;
}

