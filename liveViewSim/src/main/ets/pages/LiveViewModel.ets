export class CRiderStatus {
  m_Status: string;
  m_Distance: number;
  m_Time: number;

  constructor(vStatus: string = "骑手已接单", vDistance: number = 300, vTime: number = 3) {
    this.m_Status = vStatus;
    this.m_Distance = vDistance;
    this.m_Time = vTime;
  }
}