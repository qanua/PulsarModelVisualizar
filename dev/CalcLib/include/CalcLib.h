#pragma once

#ifdef CALCLIB_EXPORTS
#define CALCLIB_API __declspec(dllexport)
#else
#define CALCLIB_API __declspec(dllimport)
#endif

extern "C" {

    /** @brief InclinationAngleの設定
     *
     *  @param[in]      degree          パルサーの回転軸と磁化軸のなす角 [degree]
     */
    CALCLIB_API void setInclinationAngle(int degree);


    /** @brief 磁力線の頂点を取得
     *
     *  @param[out]     buffer          値を格納するfloat配列
     * 
     *  @return         格納するデータ数
     */
    CALCLIB_API int getMagneticLine(float* buffer);


    /** @brief ポーラーキャップ北極側の開磁力線の頂点を取得
     *
     *  @param[out]     buffer          値を格納するfloat配列
     *
     *  @return         格納するデータ数
     */
    CALCLIB_API int getPolarCapNorthOpened(float* buffer);


    /** @brief ポーラーキャップ北極側の閉磁力線の頂点を取得
     *
     *  @param[out]     buffer          値を格納するfloat配列
     *
     *  @return         格納するデータ数
     */
    CALCLIB_API int getPolarCapNorthClosed(float* buffer);


    /** @brief ポーラーキャップ南極側の開磁力線の頂点を取得
     *
     *  @param[out]     buffer          値を格納するfloat配列
     *
     *  @return         格納するデータ数
     */
    CALCLIB_API int getPolarCapSouthOpened(float* buffer);


    /** @brief ポーラーキャップ南極側の閉磁力線の頂点を取得
     *
     *  @param[out]     buffer          値を格納するfloat配列
     *
     *  @return         格納するデータ数
     */
    CALCLIB_API int getPolarCapSouthClosed(float* buffer);


    /** @brief スカイマップの頂点を取得
     *
     *  @param[out]     buffer          値を格納するfloat配列
     *
     *  @return         格納するデータ数
     */
    CALCLIB_API int getSkyMap(float* buffer);


    /** @brief パルスプロファイルの頂点を取得
     *
     *  @param[in]      normalize       正規化の有無
     *  @param[out]     buffer          値を格納するfloat配列
     *
     *  @return         格納するデータ数
     */
    CALCLIB_API int getPulseProfile(bool normalize, float* buffer);
}
